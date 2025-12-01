#include "PhxGrabber.h"
#include <sstream>

PhxGrabber::PhxGrabber() {
    std::memset(&mCxpRegs, 0, sizeof(mCxpRegs));
}

PhxGrabber::~PhxGrabber() {
    try {
        StopStreaming();
        Close();
    } catch (...) {
        // swallow exceptions on destructor
    }
}

void PhxGrabber::Init() {
    Options opt;
    this->Init(opt);
}

void PhxGrabber::Init(const Options& opt) {
    mOpt = opt;
    std::memset(&mCxpRegs, 0, sizeof(mCxpRegs));
    mGeometryKnown = false;

    if (!mOpt.configFile.empty()) {
        // If the same config file contains CXP register block, parse it now.
        // Failure isn’t fatal unless we depend on start/stop regs.
        PhxCommonParseCxpRegs(mOpt.configFile.c_str(), &mCxpRegs); // returns 0 on success
    }
}

void PhxGrabber::Open() {
    if (mOpened) return;

    mChannel.Open(mOpt.configFile.c_str(),
                  mOpt.boardNumber,
                  mOpt.channelNumber,
                  mOpt.configMode,
                  mOpt.acquisitionBufferCount);

    // reset context
    mCtx.PhxChannel = &mChannel;
    mCtx.owner      = this;
    mCtx.frameCv    = &mFrameCv;

    mCtx.dwBufferCount.store(0);
    mCtx.dwFifoOverflowCount.store(0);
    mCtx.dwSyncLossCount.store(0);

    // register interrupt callback
    mChannel.SetInterruptCallbackFunction(&PhxGrabber::InterruptThunk, &mCtx);

    enableInterrupts();   // ALWAYS re-enable on every Open
    ensureCxpReady();     // re-discovery each time
    applyCxpRegsIfAny();  // push regs (ROI/start/stop) if present

    // optional: configure continuous acquisition
    if (mOpt.continuousAcquisition) {
        mChannel.PhxParameterSet(PHX_ACQ_CONTINUOUS, PHX_ENABLE);
    }

    mPrevBufferCount = 0;
    mFrameIndex = 0;
    mGeometryKnown = false;
    mOpened = true;
}

void PhxGrabber::Start() {
    if (!mOpened || mStarted) return;

    // fresh counters so GetBuffer() doesn’t compare to stale values
    mPrevBufferCount = 0;
    mCtx.dwBufferCount = 0;
    mCtx.dwFifoOverflowCount = 0;
    mCtx.dwSyncLossCount = 0;

    mChannel.AcquisitionStart();

    if (mCxpRegs.dwAcqStartAddress) {
        mChannel.CxpCameraWrite(mCxpRegs.dwAcqStartAddress,
                                mCxpRegs.dwAcqStartValue);
    }

    // cache geometry once we know the acquisition has started/config is valid
    queryGeometryIfNeeded();

    mStarted = true;
}


void PhxGrabber::queryGeometryIfNeeded() {
    if (mGeometryKnown || !mOpened) return;

    ui32 w = 0, h = 0;
    ui32 srcDepthBits = 0;
    etParamValue srcCol = static_cast<etParamValue>(0);
    ui32 dstAlignBytes = 1;

    mChannel.PhxParameterGet(PHX_BUF_DST_XLENGTH, &w);
    mChannel.PhxParameterGet(PHX_BUF_DST_YLENGTH, &h);
    mChannel.PhxParameterGet(PHX_CAM_SRC_DEPTH,  &srcDepthBits); // 8/10/12/16
    mChannel.PhxParameterGet(PHX_CAM_SRC_COL,    &srcCol);
    mChannel.PhxParameterGet(PHX_DST_ALIGNMENT,  &dstAlignBytes); // row alignment in bytes

    if (dstAlignBytes == 0) dstAlignBytes = 1;

    const ui32 pxBytes = (srcDepthBits == 0 || srcDepthBits <= 8) ? 1u : 2u;
    const ui32 bytesPerLine = w * pxBytes;
    const ui32 strideBytes  = AlignUp(bytesPerLine, dstAlignBytes);
    const ui32 totalBytes   = strideBytes * h;

    mWidth         = w;
    mHeight        = h;
    mSrcDepthBits  = (srcDepthBits == 0) ? 8u : srcDepthBits;
    mSrcCol        = srcCol;
    mDstAlignBytes = dstAlignBytes;
    mBytesPerPixel = pxBytes;
    mStrideBytes   = strideBytes;
    mTotalBytes    = totalBytes;
    mGeometryKnown = true;
}

PhxGrabber::Frame PhxGrabber::GetBuffer() {
    Frame out{};
    if (!mOpened || !mStarted) return out;

    const ui32 now = mCtx.dwBufferCount;
    const i32 avail = static_cast<i32>(now - mPrevBufferCount);
    if (avail <= 0) return out;

    ui32 w = 0, h = 0;
    ui32 srcDepthBits = 0;
    etParamValue srcCol = static_cast<etParamValue>(0);

    mChannel.PhxParameterGet(PHX_BUF_DST_XLENGTH, &w);
    mChannel.PhxParameterGet(PHX_BUF_DST_YLENGTH, &h);
    mChannel.PhxParameterGet(PHX_CAM_SRC_DEPTH,  &srcDepthBits); // 8/10/12/16
    mChannel.PhxParameterGet(PHX_CAM_SRC_COL,    &srcCol);

    // Drain older frames; keep only newest
    for (i32 toDrop = avail - 1; toDrop > 0; --toDrop) {
        mChannel.BufferRelease();
    }

    void* buf = mChannel.BufferGet();
    ++mFrameIndex;

    // bytes-per-pixel in the *destination* Phoenix 
    const ui32 pxBytes = (srcDepthBits == 0 || srcDepthBits <= 8) ? 1u : 2u;

    // buffer as tightly packed, no padding.
    const ui32 strideBytes = w * pxBytes;
    const ui32 totalBytes  = strideBytes * h;

    out.data         = buf;
    out.width        = w;
    out.height       = h;
    out.strideBytes  = strideBytes;
    out.bitsPerPixel = (srcDepthBits == 0) ? 8u : srcDepthBits;
    out.srcCol       = srcCol;
    out.bytes        = totalBytes;
    out.index        = mFrameIndex;

    out.release = [this]() { this->mChannel.BufferRelease(); };

    mPrevBufferCount = now;
    return out;
}


PhxGrabber::Frame PhxGrabber::WaitForNextFrame(int timeoutMs) {
    if (!mOpened || !mStarted) {
        return Frame{};
    }

    // If timeout==0 => behave like non-blocking
    if (timeoutMs == 0) {
        return GetBuffer();
    }

    const ui32 startCount = mCtx.dwBufferCount.load();

    std::unique_lock<std::mutex> lock(mFrameMutex);
    auto pred = [&]() {
        return (mCtx.dwBufferCount.load() != startCount) || !mStarted;
    };

    if (timeoutMs < 0) {
        // infinite wait
        mFrameCv.wait(lock, pred);
    } else {
        if (!mFrameCv.wait_for(lock,
                               std::chrono::milliseconds(timeoutMs),
                               pred)) {
            // timeout
            return Frame{};
        }
    }

    if (!mStarted) return Frame{};
    return GetBuffer();
}

void PhxGrabber::streamingLoop(FrameCallback cb, int timeoutMs) {
    while (!mStopStreamingThread.load()) {
        Frame f = WaitForNextFrame(timeoutMs);
        if (!f) {
            // timeout or stopped; just continue
            continue;
        }

        try {
            cb(f);
        } catch (const std::exception& e) {
            std::cerr << "Frame callback threw exception: " << e.what() << "\n";
        } catch (...) {
            std::cerr << "Frame callback threw unknown exception\n";
        }

        if (f.release) {
            f.release();
        }
    }
    mStreaming = false;
}

void PhxGrabber::StartStreaming(FrameCallback cb, int timeoutMs) {
    if (!cb) {
        throw std::invalid_argument("PhxGrabber::StartStreaming: callback is empty");
    }

    if (!mOpened) {
        Open();
    }
    if (!mStarted) {
        Start();
    }

    // Stop any existing streaming thread
    StopStreaming();

    mStopStreamingThread = false;
    mStreaming = true;

    if (timeoutMs < 0) {
        timeoutMs = static_cast<int>(mOpt.frameWaitTimeoutMs);
    }

    mStreamThread = std::thread([this, cb, timeoutMs]() {
        streamingLoop(cb, timeoutMs);
    });
}

void PhxGrabber::StopStreaming() {
    if (!mStreaming.load()) return;

    mStopStreamingThread = true;
    mFrameCv.notify_all();  // wake up any waits

    if (mStreamThread.joinable()) {
        mStreamThread.join();
    }

    mStreaming = false;
    mStopStreamingThread = false;
}

void PhxGrabber::Close() {
    // ensure streaming is stopped
    StopStreaming();

    if (!mOpened) return;

    // 1) Stop camera streaming
    if (mStarted && mCxpRegs.dwAcqStopAddress) {
        mChannel.CxpCameraWrite(mCxpRegs.dwAcqStopAddress,
                                mCxpRegs.dwAcqStopValue);
    }

    // 2) Stop the grabber acquisition
    if (mStarted) {
        mChannel.AcquisitionStop();
        mStarted = false;
    }

    // 3) Disable interrupts, clear callback (prevents stale firing on reopen)
    try {
        ui32 zero = 0;
        mChannel.PhxParameterSet(PHX_INTRPT_SET, zero);
    } catch (...) {}

    try {
        mChannel.SetInterruptCallbackFunction(nullptr, nullptr);
    } catch (...) {}

    // 4) Sanity check: if we still own a buffer, release it
    try { mChannel.BufferRelease(); } catch (...) {}

    // 5) Close the channel
    mChannel.Close();
    mOpened = false;

    // 6) Reset counters/context after close
    mCtx.PhxChannel = nullptr;
    mCtx.owner      = nullptr;
    mCtx.frameCv    = nullptr;

    mCtx.dwBufferCount.store(0);
    mCtx.dwFifoOverflowCount.store(0);
    mCtx.dwSyncLossCount.store(0);

    mPrevBufferCount = 0;
    mFrameIndex = 0;
    mGeometryKnown = false;
}

// HELpers

void PHX_C_CALL PhxGrabber::InterruptThunk(ui32 mask, void* ctx) {
    if (!ctx) return;
    auto* c = static_cast<CallbackContext*>(ctx);

    if (PHX_INTRPT_BUFFER_READY & mask) {
        ++c->dwBufferCount;
        if (c->frameCv) {
            c->frameCv->notify_one();
        }
    }

    if (PHX_INTRPT_FIFO_OVERFLOW & mask) {
        ++c->dwFifoOverflowCount;
        std::cout << "FIFO Overflow count: " << c->dwFifoOverflowCount.load() << std::endl;
        if (c->PhxChannel) {
            c->PhxChannel->AcquisitionStop();
            c->PhxChannel->AcquisitionStart();
        }
    }

    if (PHX_INTRPT_SYNC_LOST & mask) {
        ++c->dwSyncLossCount;
        std::cout << "Sync Loss count: " << c->dwSyncLossCount.load() << std::endl;
        if (c->PhxChannel) {
            c->PhxChannel->AcquisitionStop();
            c->PhxChannel->AcquisitionStart();
        }
    }
}

void PhxGrabber::enableInterrupts() {
    etParamValue camType{};
    mChannel.PhxParameterGet(PHX_CAM_TYPE, &camType);

    ui32 intr = PHX_INTRPT_BUFFER_READY | PHX_INTRPT_FIFO_OVERFLOW;

    if (camType == PHX_CAM_AREASCAN_ROI) {
        intr |= PHX_INTRPT_SYNC_LOST;
    }

    // include global enable to be explicit
    intr |= PHX_INTRPT_GLOBAL_ENABLE;

    mChannel.PhxParameterSet(PHX_INTRPT_SET, intr);
}

void PhxGrabber::ensureCxpReady() {
    tFlag isCxp{};
    if (PHX_OK != PhxCommonIsCxp(mChannel.GetHandle(), &isCxp)) {
        throw std::runtime_error("Failed retrieving Camera Interface Type.");
    }
    if (!isCxp) return;

    ui32 discovered = 0;
    PhxCommonGetCxpDiscoveryStatus(mChannel.GetHandle(), 10, &discovered);
    if (!discovered) {
        throw std::runtime_error("Failed to discover CXP camera.");
    }
}

void PhxGrabber::applyCxpRegsIfAny() {
    // Push whatever we parsed; OK if all zeros
    if (PHX_OK != PhxCommonUpdateCxpRegs(mChannel.GetHandle(), &mCxpRegs)) {
        if (mCxpRegs.dwAcqStartAddress || mCxpRegs.dwAcqStopAddress) {
            throw std::runtime_error("Failed updating Camera CXP registers.");
        }
    }
}