#pragma once

// STL
#include <iostream>
#include <cstdlib>
#include <random>
#include <chrono>
#include <ctime>
#include <omp.h>

// X3
#include <X3.h>

// Project
#include <PhxGrabber.h>
#include <CamTex.h>
#include <DemosaicFbo.h>

struct CamProp
{
    int patternIdx = 0;
    float black=0.0f; 
    float gamma=2.2f;
    float wbR=2.0f; 
    float wbG=1.0f; 
    float wbB=1.6f; 
};

class Framer {
public:
    explicit Framer(const int channel = 1, const int board = 1);
    ~Framer();

    // Called main/render loop each frame
    void Update(const CamProp& prop);

    // Explicit shutdown
    void Kill();

    // Demosaiced RGB texture
    GLuint Texture(int& w, int& h) const;

    // Access to latest raw buffer
    // Returns true if a buffer is available; fills out fields.
    bool GetLatestRaw(uint8_t*& data, int& w, int& h, int& strideBytes);

private:
    // Worker loop running in background thread
    void WorkerLoop();

    // Called from worker thread when a new frame arrives
    void OnFrame(const PhxGrabber::Frame& f);

    // Core objects
    std::shared_ptr<PhxGrabber> mGrabber;
    std::shared_ptr<DemosaicFbo> mDem;
    std::shared_ptr<CamTex>      mTex;

    // Worker thread
    std::thread        mWorker;
    std::atomic<bool>  mRunning{false};

    // Latest raw frame (Bayer) in CPU memory (shared between threads)
    std::mutex              mImgMutex;
    std::vector<uint8_t>    mImgBuf;      // raw Bayer bytes (full stride)
    int                     mImgW      = 0;
    int                     mImgH      = 0;
    int                     mImgStride = 0;  // bytes per line from Phoenix
    int                     mImgBpp    = 8;  // bits per pixel (8/10/12/etc.)

    // Versioning to know if there is a new frame
    std::atomic<uint64_t>   mImgVersion{0};  // increments in worker
    uint64_t                mLastUsedVersion = 0; // used in render thread

    // Last exposed camera properties
    CamProp                 mProp{};
};


namespace X3
{
    namespace geom
    {
        class Camera : public Renderable
        {
        public:
            Camera(const std::string& name, const int& channel = 1, const int& board = 1);
            ~Camera();

            void RenderUi() override;
            void Update() override;

            void Kill() override;

        protected:
            std::shared_ptr<Framer> mFramer;
            CamProp mCamProp;

            int mHeight = 0;
            int mWidth = 0;
        };
    } // namespace geom
} // namespace X3