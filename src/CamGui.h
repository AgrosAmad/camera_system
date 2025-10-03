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
};

