#include "PhxGrabber.h"
#include <iostream>
#include <sstream>

PhxGrabber::PhxGrabber() {
  std::memset(&mCxpRegs, 0, sizeof(mCxpRegs));
}

PhxGrabber::~PhxGrabber() {
  try { Close(); } catch (...) {}
}

void PhxGrabber::Init()
{
    Options opt;
    this->Init(opt);
}

void PhxGrabber::Init(const Options& opt) {
  mOpt = opt;
  std::memset(&mCxpRegs, 0, sizeof(mCxpRegs));
  if (mOpt.configFile.size() > 0) {
    // If the same config file contains CXP register block, parse it now.
    // Failure isn’t fatal unless we depend on start/stop regs.
    PhxCommonParseCxpRegs(mOpt.configFile.c_str(), &mCxpRegs); // returns 0 on success
  }
}

void PhxGrabber::Open() {
  if (mOpened) return;

  mChannel.Open(mOpt.configFile.c_str(), mOpt.boardNumber, mOpt.channelNumber,
                 mOpt.configMode, mOpt.acquisitionBufferCount);

  mCtx = {};                      // reset context
  mCtx.PhxChannel = &mChannel;
  mChannel.SetInterruptCallbackFunction(&PhxGrabber::InterruptThunk, &mCtx);

  enableInterrupts();              // ALWAYS re-enable on every Open
  ensureCxpReady();                // re-discovery each time
  applyCxpRegsIfAny();             // push regs (ROI/start/stop) if present

  mPrevBufferCount = 0;
  mFrameIndex = 0;
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
    mChannel.CxpCameraWrite(mCxpRegs.dwAcqStartAddress, mCxpRegs.dwAcqStartValue);
  }
  mStarted = true;
}

  static inline ui32 AlignUp(ui32 x, ui32 a) {
      return (a > 1) ? ( (x + (a - 1)) & ~(a - 1) ) : x;
  }

  PhxGrabber::Frame PhxGrabber::GetBuffer() {
      Frame out{};
      if (!mOpened || !mStarted) return out;

      const ui32 now = mCtx.dwBufferCount;
      const i32 avail = static_cast<i32>(now - mPrevBufferCount);
      if (avail <= 0) return out;

      // --- Query geometry / format ---
      ui32 w = 0, h = 0;
      ui32 srcDepthBits = 0;
      etParamValue srcCol = static_cast<etParamValue>(0);
      ui32 dstAlignBytes = 1; // default if not set/zero

      mChannel.PhxParameterGet(PHX_BUF_DST_XLENGTH, &w);
      mChannel.PhxParameterGet(PHX_BUF_DST_YLENGTH, &h);
      mChannel.PhxParameterGet(PHX_CAM_SRC_DEPTH,  &srcDepthBits); // 8/10/12/16
      mChannel.PhxParameterGet(PHX_CAM_SRC_COL,    &srcCol);
      mChannel.PhxParameterGet(PHX_DST_ALIGNMENT,  &dstAlignBytes); // row alignment in bytes

      // Drain older frames; keep only newest
      for (i32 toDrop = avail - 1; toDrop > 0; --toDrop) {
          mChannel.BufferRelease();
      }

      void* buf = mChannel.BufferGet();
      ++mFrameIndex;

      // Compute bytes-per-pixel for the *destination* buffer we get.
      const ui32 pxBytes = (srcDepthBits == 0 || srcDepthBits <= 8) ? 1u : 2u;

      // Derive stride using destination alignment
      if (dstAlignBytes == 0) dstAlignBytes = 1;
      const ui32 bytesPerLine = w * pxBytes;
      const ui32 strideBytes  = AlignUp(bytesPerLine, dstAlignBytes);
      const ui32 totalBytes   = strideBytes * h;

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


void PhxGrabber::Close() {
  if (!mOpened) return;

  // 1) Stop camera streaming
  if (mStarted && mCxpRegs.dwAcqStopAddress) {
    mChannel.CxpCameraWrite(mCxpRegs.dwAcqStopAddress, mCxpRegs.dwAcqStopValue);
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
  std::memset(&mCtx, 0, sizeof(mCtx));
  mPrevBufferCount = 0;
  mFrameIndex = 0;
}

// ----- helpers -----

void PHX_C_CALL PhxGrabber::InterruptThunk(ui32 mask, void* ctx) {
  if (!ctx) return;
  auto* c = static_cast<CallbackContext*>(ctx);

  if (PHX_INTRPT_BUFFER_READY & mask) {
    ++c->dwBufferCount;
  }
  if (PHX_INTRPT_FIFO_OVERFLOW & mask) {
    ++c->dwFifoOverflowCount;
    std::cout << "FIFO Overflow count: " << c->dwFifoOverflowCount << std::endl;
    c->PhxChannel->AcquisitionStop();
    c->PhxChannel->AcquisitionStart();
  }
  if (PHX_INTRPT_SYNC_LOST & mask) {
    ++c->dwSyncLossCount;
    std::cout << "Sync Loss count: " << c->dwSyncLossCount << std::endl;
    c->PhxChannel->AcquisitionStop();
    c->PhxChannel->AcquisitionStart();
  }
}

void PhxGrabber::enableInterrupts() {
  ui32 intr = PHX_INTRPT_FIFO_OVERFLOW;
  etParamValue camType{};
  mChannel.PhxParameterGet(PHX_CAM_TYPE, &camType);
  if (camType == PHX_CAM_AREASCAN_ROI) intr |= PHX_INTRPT_SYNC_LOST;
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
