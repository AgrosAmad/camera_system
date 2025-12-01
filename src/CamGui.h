#pragma once

// Project
#include <CamObj.h>

class CamGui
{
    public:

        // Constructors & Destructors
        CamGui();
        ~CamGui() {};

        // Run app
        void Run();
    private:
        // X3 Engine
        std::shared_ptr<X3::Engine> mEngine;

        // Camera obj
        std::shared_ptr<X3::geom::Camera> mCamObj;
};

