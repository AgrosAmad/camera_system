#pragma once

// STL
#include <filesystem>
#include <string>
#include <map>

// GL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// XE
#include <Parameters.h>
#include <Viewport.h>
#include <Input.h>
#include <Scene.h>
#include <Gui.h>

namespace X3
{
    class Engine
    {
    public:
        // Main functions
        Engine(const std::string& windowTitle = "X3", WindowScreen screenAspect = SMALL);
        virtual ~Engine() {};
        void Run();

        // Handle viewports
        std::shared_ptr<Viewport> AddViewport(const std::string& name = "Viewport");
        void RemoveViewport(std::shared_ptr<Viewport> v);
        void AddViewport(std::shared_ptr<Viewport> v);

        // Get/Set
        std::vector<std::shared_ptr<Viewport>> Viewports() { return mViewports; }
        std::shared_ptr<Scene>& GetScene() { return mScene; }

        // Helpers to Add/Remove objects
        void LoadShader(const std::string& key, const std::vector<std::string>& files);

    private:
        // Data
        std::vector<std::shared_ptr<Viewport>> mViewports; // In-window viewports
        GLFWwindow* mWindow = nullptr; // App window
        std::shared_ptr<Scene> mScene; // Objects
        std::shared_ptr<Gui> mUi; // Graphic interface

        // Main loop
        void Update();
        void Render();

        // Helpers
        void InitOpenGL(const std::string& windowTitle, WindowScreen screenAspect);
        void InitParams(const WindowScreen& screenAspect);
        void InitCallbacks();

        // Input callbacks
        void CallbackError(int error, const char* description);
        void CallbackKey(GLFWwindow* window, int key, int scancode, int action, int mods);
        void CallbackMouse(GLFWwindow* window, double x, double y);
        void CallbackMouseButton(GLFWwindow* window, int button, int action, int mods);
        void CallbackMouseScroll(GLFWwindow* window, double x, double y);
        void CallbackFramebufferSize(GLFWwindow* window, int x, int y);
        void CallbackWindowSize(GLFWwindow* window, int x, int y);
        void CallbackFocus(GLFWwindow* window, int focused);
    };
} // namespace X3