#include "CamObj.h"
#include <chrono>
#include <cstring>   // std::memcpy
#include <iostream>  // debugging

using namespace std::chrono_literals;

Framer::Framer(const int channel, const int board)
{
    // --- Create and configure grabber ---
    PhxGrabber::Options opt;
    opt.channelNumber = static_cast<ui32>(channel);
    // adapt these if needed:
    opt.boardNumber   = static_cast<ui32>(board);
    opt.configMode    = PHX_CONFIG_NORMAL;
    opt.acquisitionBufferCount = 10;

    mGrabber = std::make_shared<PhxGrabber>();
    mGrabber->Init(opt);
    mGrabber->Open();
    mGrabber->Start();   // non-streaming; poll in own thread

    // GL helpers 
    mTex = std::make_shared<CamTex>();
    mDem = std::make_shared<DemosaicFbo>();
    mDem->Init();

    // Worker
    mRunning = true;
    mWorker = std::thread(&Framer::WorkerLoop, this);
}

Framer::~Framer()
{
    Kill();
}

void Framer::Kill()
{
    bool expected = true;
    if (mRunning.compare_exchange_strong(expected, false)) {
        // Stop worker
        if (mWorker.joinable())
            mWorker.join();

        // Stop & close grabber
        if (mGrabber) {
            try {
                mGrabber->Close();
            } catch (...) {
                // swallow in destructor
            }
        }
    }
}

void Framer::WorkerLoop()
{
    // This runs on background thread.
    while (mRunning) {
        PhxGrabber::Frame f = mGrabber->GetBuffer(); // non-blocking
        if (f) {
            OnFrame(f); // copies to CPU buffer & releases SDK buffer
        } else {
            // No new frame yet
            std::this_thread::sleep_for(1ms);
        }
    }
}

void Framer::OnFrame(const PhxGrabber::Frame& f)
{


    // Worker thread: copy into mImgBuf (CPU) & release Phoenix buffer.
    if (!f.data || f.width == 0 || f.height == 0 || f.bytes == 0) {
        if (f.release) f.release();
        return;
    }

    const int w   = static_cast<int>(f.width);
    const int h   = static_cast<int>(f.height);
    const int bpp = static_cast<int>(f.bitsPerPixel);
    const int stride = static_cast<int>(f.strideBytes); // bytes per line

    // Allocate CPU buffer large enough for full stride * height
    const size_t needed = static_cast<size_t>(stride) * static_cast<size_t>(h);

    {
        std::lock_guard<std::mutex> lock(mImgMutex);

        if (mImgBuf.size() != needed)
            mImgBuf.resize(needed);

        // Copy full buffer
        std::memcpy(mImgBuf.data(), f.data, needed);

        mImgW      = w;
        mImgH      = h;
        mImgStride = stride;
        mImgBpp    = bpp;

        // Increment version so render thread knows a new frame is ready
        mImgVersion.fetch_add(1, std::memory_order_release);
    }

    // Return buffer to SDK
    if (f.release) f.release();
}

void Framer::Update(const CamProp& prop)
{
    // Called from render/UI thread *with current GL context*.
    mProp = prop; // store latest prop used for demosaic

    const uint64_t v = mImgVersion.load(std::memory_order_acquire);
    if (v == mLastUsedVersion) {
        // No new frame since we last updated
        return;
    }

    // Snapshot CPU buffer under lock
    std::vector<uint8_t> localBuf;
    int w = 0, h = 0, stride = 0, bpp = 0;
    {
        std::lock_guard<std::mutex> lock(mImgMutex);
        if (mImgBuf.empty() || mImgW == 0 || mImgH == 0) {
            return;
        }
        localBuf = mImgBuf; // copy (small overhead, but keeps life simple)
        w        = mImgW;
        h        = mImgH;
        stride   = mImgStride;
        bpp      = mImgBpp;
    }
    mLastUsedVersion = v;

    // For now, we assume Bayer8 (1 byte/px).
    if (bpp != 8) {
        // TODO: repack logic for >8bpp.
        std::cerr << "[Framer] bpp=" << bpp << " not yet handled in texture upload\n";
        return;
    }

    // Upload to GPU as R8 grayscale (Bayer mosaic)
    mTex->updateRaw(localBuf.data(), w, h, stride);

    // Run demosaic into RGB FBO
    auto pat = static_cast<BayerPattern>(mProp.patternIdx);
    mDem->Render(
        mTex->id,
        mTex->w,
        mTex->h,
        pat,
        mProp.black,
        mProp.wbR,
        mProp.wbG,
        mProp.wbB,
        mProp.gamma
    );
}

GLuint Framer::Texture(int &w, int& h) const
{
    // Texture dimensions
    w = mTex->w;
    h = mTex->h;

    // Final demosaiced color texture
    return mDem ? mDem->ColorTexture() : 0;
}

bool Framer::GetLatestRaw(uint8_t*& data, int& w, int& h, int& strideBytes)
{
    std::lock_guard<std::mutex> lock(mImgMutex);
    if (mImgBuf.empty() || mImgW == 0 || mImgH == 0)
        return false;

    data        = mImgBuf.data();
    w           = mImgW;
    h           = mImgH;
    strideBytes = mImgStride;
    return true;
}


namespace X3
{
    namespace geom
    {
        Camera::Camera(const std::string& name, const int& channel, const int& board)
        {
            // Inits camera and framer
            mFramer = std::make_shared<Framer>(channel, board);
            mName = name;
        }

        Camera::~Camera()
        {

        }

        void Camera::RenderUi()
        {

            // Gets latest texture
            auto image = mFramer->Texture(mWidth, mHeight);

            
            ImGui::Text("Name: %s", mName.c_str());
            ImGui::Text("Size: %i, %i", mWidth, mHeight);
            ImGui::Image((ImTextureID)(intptr_t)image, ImVec2(512, 512));
        }

        void Camera::Kill()
        {
            mFramer->Kill();
        }

        void Camera::Update()
        {
            mFramer->Update(mCamProp);
        }
    }
}