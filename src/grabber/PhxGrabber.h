#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <stdexcept>
#include <cstring>  
#include <fstream> 
#include <iostream> 

#include "phx_core.h"
#include "common.h"

// Lifecycle: Init -> Open -> Start -> (loop: GetBuffer -> use -> frame.release()) -> Close
class PhxGrabber {
    public:
    // Init-time options (no display; just capture)
    struct Options {
        std::string  configFile = std::string(PROJECT_ROOT) + "/cam_config.pcf";       // .cam/.cfg or nullptr
        ui32         boardNumber = 1;
        ui32         channelNumber = 1;
        etParamValue configMode = PHX_CONFIG_NORMAL;
        ui32         acquisitionBufferCount = 10; // number of DMA buffers
    };

    // A lightweight handle to the newest acquired frame.
    // IMPORTANT: call release() when done reading the pixels.
    struct Frame {
        const void* data = nullptr;
        ui32 width = 0;
        ui32 height = 0;
        ui32 bytes = 0;          // total bytes in buffer
        ui32 strideBytes = 0;    // bytes per line (important if padding)
        ui32 bitsPerPixel = 8;   // 8/10/12/16 raw
        etParamValue srcCol = static_cast<etParamValue>(0); // PHX_CAM_SRC_*?
        ui64 index = 0;
        std::function<void()> release;
        explicit operator bool() const { return data != nullptr; }
    };


    PhxGrabber();
    ~PhxGrabber();

    void Init();
    void Init(const Options& opt);     // stores options; parses any CXP regs if provided
    void Open();                       // opens board/channel, sets interrupts, discovery
    void Start();                      // starts acquisition (and CXP start reg if present)
    Frame GetBuffer();                 // non-blocking: returns newest frame; releases older
    void Close();                      // stops (if needed) and closes

    // Stats (debug)
    ui32 bufferCount() const      { return mCtx.dwBufferCount; }
    ui32 fifoOverflowCount() const{ return mCtx.dwFifoOverflowCount; }
    ui32 syncLossCount() const    { return mCtx.dwSyncLossCount; }

    bool SaveFrameRaw(const Frame& f, const std::string& path, bool autoRelease) {
    if (!f.data || f.width == 0 || f.height == 0 || f.bytes == 0) {
        std::cerr << "SaveFrameRaw: invalid frame\n";
        return false;
    }

    std::ofstream os(path, std::ios::binary);
    if (!os) {
        std::cerr << "SaveFrameRaw: cannot open '" << path << "' for writing\n";
        if (autoRelease) const_cast<Frame&>(f).release();
        return false;
    }

    os.write(static_cast<const char*>(f.data), static_cast<std::streamsize>(f.bytes));
    os.close();

    if (autoRelease) {
        const_cast<Frame&>(f).release();   // return buffer to SDK
    }
    return true;
}

bool SaveLatestRaw(const std::string& path) {
    Frame f = GetBuffer();          // non-blocking!!; may be empty if no new frame yet
    if (!f) return false;
    return SaveFrameRaw(f, path, /*autoRelease=*/true);
}

    private:
    // Interrupt context
    struct CallbackContext {
        CPhxCore* PhxChannel = nullptr;
        ui32 dwBufferCount = 0;
        ui32 dwFifoOverflowCount = 0;
        ui32 dwSyncLossCount = 0;
    };

    static void PHX_C_CALL InterruptThunk(ui32 mask, void* ctx);

    // helpers
    void enableInterrupts();
    void ensureCxpReady();
    void applyCxpRegsIfAny();

    Options        mOpt{};
    CPhxCore       mChannel{};
    CallbackContext mCtx{};
    tCxpRegisters   mCxpRegs{}; // parsed from config file if provided

    bool mOpened  = false;
    bool mStarted = false;
    ui32 mPrevBufferCount = 0;
    ui64 mFrameIndex = 0;
};