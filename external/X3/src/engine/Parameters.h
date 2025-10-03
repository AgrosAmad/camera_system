#pragma once

// STL
#include <string>
#include <map>
#include <memory>

// GL
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Options
enum CursorMode { NORMAL = GLFW_CURSOR_NORMAL, HIDDEN = GLFW_CURSOR_HIDDEN, DISABLED = GLFW_CURSOR_DISABLED };
enum WindowScreen { SMALL, MAXIMIZED, FULL };

// Singleton class to manage engine parameters
namespace X3
{
    class Parameters
    {
    public:

        // Gets only instance
        static Parameters& Inst();

        // Setting wrapers
        struct
        {
            std::string RootDir; // current work directory
            float MouseSens; // mouse sensitivity
            float ScrollSens; // mouse scrolling sensitivity
            float PanningSens; // mouse panning sensitivity
            bool GlCheckErrors; // check for opengl errors     
            bool ShaderAutoReload; // automatically reload shader when modified
            bool ShowGui; // show gui elements
            bool ShowGuiMenuBar;
            bool ShowGuiLog;
        } Main;

        struct
        {
            GLsizei SamplesMS; // number of multisamples
            glm::ivec2 FrameBufferDim; // the size of the default framebuffer
            glm::ivec2 WindowDim; // the size of the window
            CursorMode mCursorMode; // drawing mode of the mouse cursor
            WindowScreen mWindowScreen; // screen size
            bool Shadows;
            bool RenderBlending;
            glm::vec3 BGColor;
            glm::vec2 HdrExpGam;
            glm::vec3 LightAttenuation;
        } RenderSettings;

        struct
        {
            float DtRender; // time per frame render
            float DtUpdate; // time per scene update
            float DtApp; // time per app cycle
            float Spf; // moving average of seconds per frame
        } Profiler;

        float ScreenRatio() { return RenderSettings.FrameBufferDim.x / (float)RenderSettings.FrameBufferDim.y; }
        float WindowRatio() { return RenderSettings.WindowDim.x / (float)RenderSettings.WindowDim.y; }
        glm::ivec2 ScreenDim() { return RenderSettings.FrameBufferDim; }
        glm::ivec2 WindowDim() { return RenderSettings.WindowDim; }

    private:
        // Private constructor to make class singleton
        Parameters()
        {
            Main.PanningSens = 0.04;
            Main.MouseSens = 0.02f;
            Main.ScrollSens = 0.5f;
            Main.GlCheckErrors = false;
            Main.ShaderAutoReload = true;
            Main.ShowGui = true;
            Main.ShowGuiMenuBar = false;
            Main.ShowGuiLog = true;

            RenderSettings.SamplesMS = 4;
            RenderSettings.FrameBufferDim = glm::ivec2(0, 0);
            RenderSettings.WindowDim = glm::ivec2(0, 0);
            RenderSettings.mCursorMode = CursorMode::NORMAL;
            RenderSettings.mWindowScreen = WindowScreen::SMALL;

            RenderSettings.LightAttenuation = glm::vec3(0, 0, 1);
            RenderSettings.BGColor = glm::vec3(0.f);
            RenderSettings.HdrExpGam = glm::vec2(1.0f, 1.0f);
            RenderSettings.RenderBlending = true;
            RenderSettings.Shadows = false;

            Profiler.DtRender = 0.f;
            Profiler.DtRender = 0.f;
            Profiler.DtApp = 0.f;
            Profiler.Spf = 1.f;
        }

        Parameters(const Parameters&) = delete; // No copy constructor allowed
        void operator=(const Parameters&) = delete; // No copy assignment allowed
    };
} // namespace X3