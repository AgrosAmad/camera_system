#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <stdexcept>
#include <cstring>
#include <fstream>
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "phx_core.h"
#include "common.h"

// Lifecycle: Init -> Open -> Start ->
//   (pull:   GetBuffer / WaitForNextFrame -> use -> frame.release())
//   (push:   StartStreaming(callback) -> callback(Frame&)... -> StopStreaming)
// -> Close
class PhxGrabber {
public:
    // Init-time options (no display; just capture)
    struct Options {
        std::string  configFile = std::string(PROJECT_ROOT) + "/cam_config.pcf"; // .cam/.cfg/.pcf
        ui32         boardNumber = 1;
        ui32         channelNumber = 1;
        etParamValue configMode = PHX_CONFIG_NORMAL;
        ui32         acquisitionBufferCount = 10; // number of DMA buffers
        bool         continuousAcquisition = true;
        ui32         frameWaitTimeoutMs = 1000;   // default wait in blocking calls
    };

    // A lightweight handle to the newest acquired frame.
    // IMPORTANT: call release() when done reading the pixels (unless streaming
    // mode).
    struct Frame {
        const void* data = nullptr;
        ui32 width = 0;
        ui32 height = 0;
        ui32 bytes = 0;          // total bytes in buffer (stride * height)
        ui32 strideBytes = 0;    // bytes per line (important if padding)
        ui32 bitsPerPixel = 8;   // 8/10/12/16 raw
        etParamValue srcCol = static_cast<etParamValue>(0); // PHX_CAM_SRC_*?
        ui64 index = 0;
        std::function<void()> release;  // must be called to return buffer

        explicit operator bool() const { return data != nullptr; }
    };

    using FrameCallback = std::function<void(const Frame&)>;

    PhxGrabber();
    ~PhxGrabber();

    // Init-time configuration
    void Init();
    void Init(const Options& opt);

    // Open/close camera (board/channel) and configure interrupts & CXP
    void Open();
    void Start();    // start acquisition (and CXP start reg if present)
    void Close();    // stops streaming, stops acq, closes board


    // Non-blocking: returns newest frame if available, or an empty Frame.
    // Releases all older frames before returning the newest one.
    Frame GetBuffer();

    // Non-blocking alias (for clarity)
    Frame GetLatestNonBlocking() { return GetBuffer(); }

    // Blocking: waits for the *next* frame (i.e., a new buffer-ready interrupt)
    // timeoutMs < 0 => infinite
    // timeoutMs == 0 => non-blocking (equivalent to GetBuffer)
    Frame WaitForNextFrame(int timeoutMs = -1);


    // Start a background thread that continuously waits for frames and invokes
    // the callback for each one. The streaming loop will automatically call
    // frame.release() after the callback returns.
    //
    // If the grabber is not yet opened/started, this will call Open() and Start().
    // timeoutMs is the per-frame wait timeout in the streaming loop.
    void StartStreaming(FrameCallback cb,
                        int timeoutMs = -1 /* infinite per frame */);

    // Stop the background streaming thread (if running).
    void StopStreaming();

    // Stats (debug)
    ui32 bufferCount() const      { return mCtx.dwBufferCount.load(); }
    ui32 fifoOverflowCount() const{ return mCtx.dwFifoOverflowCount.load(); }
    ui32 syncLossCount() const    { return mCtx.dwSyncLossCount.load(); }

    bool SaveFrameRaw(const Frame& f, const std::string& path, bool autoRelease = true) {
        if (!f.data || f.width == 0 || f.height == 0 || f.bytes == 0) {
            std::cerr << "SaveFrameRaw: invalid frame\n";
            return false;
        }

        std::ofstream os(path, std::ios::binary);
        if (!os) {
            std::cerr << "SaveFrameRaw: cannot open '" << path << "' for writing\n";
            if (autoRelease && f.release) const_cast<Frame&>(f).release();
            return false;
        }

        os.write(static_cast<const char*>(f.data),
                 static_cast<std::streamsize>(f.bytes));
        os.close();

        if (autoRelease && f.release) {
            const_cast<Frame&>(f).release();   // return buffer to SDK
        }
        return true;
    }

    bool SaveLatestRaw(const std::string& path) {
        Frame f = GetBuffer();          // non-blocking; may be empty
        if (!f) return false;
        return SaveFrameRaw(f, path, /*autoRelease=*/true);
    }

private:
    // Interrupt context
    struct CallbackContext {
        CPhxCore*       PhxChannel = nullptr;
        PhxGrabber*     owner      = nullptr;
        std::atomic<ui32> dwBufferCount{0};
        std::atomic<ui32> dwFifoOverflowCount{0};
        std::atomic<ui32> dwSyncLossCount{0};
        std::condition_variable* frameCv = nullptr;  // to wake WaitForNextFrame
    };

    static void PHX_C_CALL InterruptThunk(ui32 mask, void* ctx);

    // helpers
    void enableInterrupts();
    void ensureCxpReady();
    void applyCxpRegsIfAny();
    void queryGeometryIfNeeded();    // cache w/h/stride/etc.
    static inline ui32 AlignUp(ui32 x, ui32 a) {
        return (a > 1) ? ((x + (a - 1)) & ~(a - 1)) : x;
    }

    // streaming thread loop
    void streamingLoop(FrameCallback cb, int timeoutMs);

private:
    Options         mOpt{};
    CPhxCore        mChannel{};
    CallbackContext mCtx{};
    tCxpRegisters   mCxpRegs{}; // parsed from config file if provided

    bool mOpened  = false;
    bool mStarted = false;

    // frame counters / indices
    ui32 mPrevBufferCount = 0;
    ui64 mFrameIndex = 0;

    // cached geometry
    bool         mGeometryKnown   = false;
    ui32         mWidth           = 0;
    ui32         mHeight          = 0;
    ui32         mSrcDepthBits    = 8;
    etParamValue mSrcCol          = static_cast<etParamValue>(0);
    ui32         mDstAlignBytes   = 1;
    ui32         mBytesPerPixel   = 1;
    ui32         mStrideBytes     = 0;
    ui32         mTotalBytes      = 0;

    // synchronization / streaming
    std::mutex              mFrameMutex;
    std::condition_variable mFrameCv;
    std::thread             mStreamThread;
    std::atomic<bool>       mStreaming{false};
    std::atomic<bool>       mStopStreamingThread{false};
};
