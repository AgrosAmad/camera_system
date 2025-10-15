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

                std::shared_ptr<PhxGrabber> mCamGrab;
                std::shared_ptr<CamTex> mCamTex;

                std::shared_ptr<PhxGrabber> mCamGrab2;
                std::shared_ptr<CamTex> mCamTex2;

        };
    } // namespace geom
} // namespace X3