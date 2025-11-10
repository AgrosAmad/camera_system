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

class CamCont
{
    public:
        CamCont(const int& channel = 1);
        ~CamCont();

        void Draw();
        void Update(const CamProp& prop);
        void Kill();

        GLuint Texture() const;
    private:
        std::shared_ptr<PhxGrabber> mGrabber;
        std::shared_ptr<DemosaicFbo> mDem;
        std::shared_ptr<CamTex> mTex;

        CamProp mProp;
};

namespace X3
{
    namespace geom
    {
        class CamObj : public Renderable
        {
        public:
            CamObj();
            ~CamObj();

            void RenderUi() override;
            void Update() override;

            void Kill() override;
        protected:
            std::shared_ptr<CamCont> mCam1, mCam2;
            CamProp MasterProp;
        };
    } // namespace geom
} // namespace X3