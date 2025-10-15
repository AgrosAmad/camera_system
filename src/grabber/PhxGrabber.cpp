#include "PhxGrabber.h"
#include <iostream>
#include <sstream>

PhxGrabber::PhxGrabber() {
  std::memset(&m_cxpRegs, 0, sizeof(m_cxpRegs));
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
  m_opt = opt;
  std::memset(&m_cxpRegs, 0, sizeof(m_cxpRegs));
  if (m_opt.configFile) {
    // If the same config file contains CXP register block, parse it now.
    // Failure isn’t fatal unless we depend on start/stop regs.
    PhxCommonParseCxpRegs(m_opt.configFile, &m_cxpRegs); // returns 0 on success
  }
}

void PhxGrabber::Open() {
  if (m_opened) return;

  m_channel.Open(m_opt.configFile, m_opt.boardNumber, m_opt.channelNumber,
                 m_opt.configMode, m_opt.acquisitionBufferCount);

  m_ctx = {};                      // reset context
  m_ctx.PhxChannel = &m_channel;
  m_channel.SetInterruptCallbackFunction(&PhxGrabber::InterruptThunk, &m_ctx);

  enableInterrupts();              // ALWAYS re-enable on every Open
  ensureCxpReady();                // re-discovery each time
  applyCxpRegsIfAny();             // push regs (ROI/start/stop) if present

  m_prevBufferCount = 0;
  m_frameIndex = 0;
  m_opened = true;
}

void PhxGrabber::Start() {
  if (!m_opened || m_started) return;

  // fresh counters so GetBuffer() doesn’t compare to stale values
  m_prevBufferCount = 0;
  m_ctx.dwBufferCount = 0;
  m_ctx.dwFifoOverflowCount = 0;
  m_ctx.dwSyncLossCount = 0;

  m_channel.AcquisitionStart();
  if (m_cxpRegs.dwAcqStartAddress) {
    m_channel.CxpCameraWrite(m_cxpRegs.dwAcqStartAddress, m_cxpRegs.dwAcqStartValue);
  }
  m_started = true;
}

PhxGrabber::Frame PhxGrabber::GetBuffer() {
  Frame out{};

  if (!m_opened || !m_started) return out;

  // Determine how many new frames arrived since last call
  ui32 now = m_ctx.dwBufferCount;
  i32 avail = static_cast<i32>(now - m_prevBufferCount);
  if (avail <= 0) {
    return out; // nothing new
  }

  // Get buffer geometry (monochrome/raw assumption same as sdk sample)
  ui32 w = 0, h = 0;
  m_channel.PhxParameterGet(PHX_BUF_DST_XLENGTH, &w);
  m_channel.PhxParameterGet(PHX_BUF_DST_YLENGTH, &h);
  const ui32 bytes = w * h;

  // Drain older frames; keep only the newest
  while ((--avail) > 0) {
    m_channel.BufferRelease();
  }

  // Now the next BufferGet() is the newest frame
  void* buf = m_channel.BufferGet();
  ++m_frameIndex;

  // Fill out Frame and give the caller a release() they must call.
  out.data   = buf;
  out.width  = w;
  out.height = h;
  out.bytes  = bytes;
  out.index  = m_frameIndex;
  out.release = [this]() {
    // Safe to call exactly once per acquired frame
    this->m_channel.BufferRelease();
  };

  // Update our count to the latest we returned
  m_prevBufferCount = now;
  return out;
}

void PhxGrabber::Close() {
  if (!m_opened) return;

  // 1) Stop camera streaming (if we know how)
  if (m_started && m_cxpRegs.dwAcqStopAddress) {
    m_channel.CxpCameraWrite(m_cxpRegs.dwAcqStopAddress, m_cxpRegs.dwAcqStopValue);
  }

  // 2) Stop the grabber acquisition
  if (m_started) {
    m_channel.AcquisitionStop();
    m_started = false;
  }

  // 3) Disable interrupts, clear callback (prevents stale firing on reopen)
  try {
    ui32 zero = 0;
    m_channel.PhxParameterSet(PHX_INTRPT_SET, zero);
  } catch (...) {}
  try {
    m_channel.SetInterruptCallbackFunction(nullptr, nullptr);
  } catch (...) {}

  // 4) Sanity check: if we still own a buffer, release it
  try { m_channel.BufferRelease(); } catch (...) {}

  // 5) Close the channel
  m_channel.Close();
  m_opened = false;

  // 6) Reset counters/context after close
  std::memset(&m_ctx, 0, sizeof(m_ctx));
  m_prevBufferCount = 0;
  m_frameIndex = 0;
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
  m_channel.PhxParameterGet(PHX_CAM_TYPE, &camType);
  if (camType == PHX_CAM_AREASCAN_ROI) intr |= PHX_INTRPT_SYNC_LOST;
  m_channel.PhxParameterSet(PHX_INTRPT_SET, intr);
}

void PhxGrabber::ensureCxpReady() {
  tFlag isCxp{};
  if (PHX_OK != PhxCommonIsCxp(m_channel.GetHandle(), &isCxp)) {
    throw std::runtime_error("Failed retrieving Camera Interface Type.");
  }
  if (!isCxp) return;

  ui32 discovered = 0;
  PhxCommonGetCxpDiscoveryStatus(m_channel.GetHandle(), 10, &discovered);
  if (!discovered) {
    throw std::runtime_error("Failed to discover CXP camera.");
  }
}

void PhxGrabber::applyCxpRegsIfAny() {
  // Push whatever we parsed; OK if all zeros
  if (PHX_OK != PhxCommonUpdateCxpRegs(m_channel.GetHandle(), &m_cxpRegs)) {
    if (m_cxpRegs.dwAcqStartAddress || m_cxpRegs.dwAcqStopAddress) {
      throw std::runtime_error("Failed updating Camera CXP registers.");
    }
  }
}
