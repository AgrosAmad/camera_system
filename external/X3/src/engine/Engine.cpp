// X3
#include <Materials.h>
#include <Engine.h>

namespace X3
{
    Engine::Engine(const std::string& windowTitle, WindowScreen screenAspect)
    {

        // General inits
        InitOpenGL(windowTitle, screenAspect);
        InitParams(screenAspect);
        InitCallbacks();

        // Init engine
        mUi = std::make_shared<Gui>(mWindow);
        mScene = std::make_shared<Scene>();

        // Main viewport
        std::shared_ptr<Viewport> v = AddViewport();
        v->FullScreen(true);
    }

    void Engine::Update()
    {
        Parameters& params = Parameters::Inst();

        // Profiler
        float time0 = glfwGetTime();

        // Updates viewports
        for (auto v : mViewports)
        {
            v->Update();
        }

        // Then scene
        mScene->Update();

        // Finally, managers
        Materials& materials = Materials::Inst();
        Shaders& shaders = Shaders::Inst();

        materials.Update();
        shaders.Update();

        params.Profiler.DtUpdate = glfwGetTime() - time0;
    }

    void Engine::Render()
    {

        // Profiler
        Parameters& params = Parameters::Inst();
        float time0 = glfwGetTime();

        // Render only if window not minimized
        if (params.RenderSettings.WindowDim.x > 0 && params.RenderSettings.WindowDim.y > 0)
        {
            // Clears background if no viewport is in fullscreen mode
            bool fullScreen = false;
            for (auto v : mViewports) fullScreen |= v->FullScreen();

            if (!fullScreen)
            {
                glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
            }

            // Render viewports
            for (auto v : mViewports) v->Render(mScene);

            // Render UI
            mUi->Render(this);
        }

        params.Profiler.DtRender = glfwGetTime() - time0;

    }

    void Engine::InitOpenGL(const std::string& windowTitle, WindowScreen screenAspect)
    {
        // Initialize OpenGL

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        const auto primaryMonitor = glfwGetPrimaryMonitor();
        const auto videoMode = glfwGetVideoMode(primaryMonitor);

        // Generate window
        mWindow = glfwCreateWindow(videoMode->width / 2, videoMode->height / 2, windowTitle.c_str(), screenAspect == FULL ? glfwGetPrimaryMonitor() : nullptr, nullptr);

        glfwMakeContextCurrent(mWindow);
        gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

        if (screenAspect == MAXIMIZED)
        {
            glfwMaximizeWindow(mWindow);
            int width, height;
            glfwGetWindowSize(mWindow, &width, &height);
        }
    }

    void Engine::InitCallbacks()
    {
        auto func_key_callback = [](GLFWwindow* w, int key, int scancode, int action, int mods)
            {static_cast<Engine*>(glfwGetWindowUserPointer(w))->CallbackKey(w, key, scancode, action, mods); };

        auto func_cursor_pos_callback = [](::GLFWwindow* w, double x, double y)
            {static_cast<Engine*>(glfwGetWindowUserPointer(w))->CallbackMouse(w, x, y); };

        auto func_mouse_button_callback = [](::GLFWwindow* w, int button, int action, int mods)
            {static_cast<Engine*>(glfwGetWindowUserPointer(w))->CallbackMouseButton(w, button, action, mods); };

        auto func_scroll_callback = [](::GLFWwindow* w, double x, double y)
            {static_cast<Engine*>(glfwGetWindowUserPointer(w))->CallbackMouseScroll(w, x, y); };

        auto func_fb_size_callback = [](::GLFWwindow* w, int x, int y)
            {static_cast<Engine*>(glfwGetWindowUserPointer(w))->CallbackFramebufferSize(w, x, y); };

        auto func_window_size_callback = [](::GLFWwindow* w, int x, int y)
            {static_cast<Engine*>(glfwGetWindowUserPointer(w))->CallbackWindowSize(w, x, y); };

        auto func_window_focus_callback = [](::GLFWwindow* w, int focused)
            {static_cast<Engine*>(glfwGetWindowUserPointer(w))->CallbackFocus(w, focused); };

        glfwSetKeyCallback(mWindow, func_key_callback);
        glfwSetCursorPosCallback(mWindow, func_cursor_pos_callback);
        glfwSetMouseButtonCallback(mWindow, func_mouse_button_callback);
        glfwSetScrollCallback(mWindow, func_scroll_callback);
        glfwSetFramebufferSizeCallback(mWindow, func_fb_size_callback);
        glfwSetWindowSizeCallback(mWindow, func_window_size_callback);
        glfwSetWindowFocusCallback(mWindow, func_window_focus_callback);
    }

    void Engine::InitParams(const WindowScreen& screenAspect)
    {
        // initialize params
        Parameters& params = Parameters::Inst();
        glfwGetFramebufferSize(mWindow, &params.RenderSettings.FrameBufferDim.x, &params.RenderSettings.FrameBufferDim.y);
        glfwGetWindowSize(mWindow, &params.RenderSettings.WindowDim.x, &params.RenderSettings.WindowDim.y);

        // Sets root directory and window settings
        params.Main.RootDir = std::filesystem::path(__FILE__).parent_path().parent_path().parent_path().string();
        params.RenderSettings.mWindowScreen = screenAspect;
    }

    void Engine::CallbackError(int error, const char* description)
    {
        fputs(description, stderr);
    }

    void Engine::CallbackKey(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
        Input::Inst().KeyCallback(key, scancode, action, mods);
    }

    void Engine::CallbackMouse(GLFWwindow* window, double x, double y)
    {
        Parameters& p = Parameters::Inst();
        auto sx = double(p.ScreenDim().x) / double(p.WindowDim().x);
        auto sy = double(p.ScreenDim().y) / double(p.WindowDim().y);

        Input::Inst().MouseCallback(x * sx, y * sy);
    }

    void Engine::CallbackMouseButton(GLFWwindow* window, int button, int action, int mods)
    {
        Input::Inst().MouseButtonCallback(button, action, mods);
    }

    void Engine::CallbackMouseScroll(GLFWwindow* window, double x, double y)
    {
        Input::Inst().MouseScrollCallback(x, y);
    }

    void Engine::CallbackFramebufferSize(GLFWwindow* window, int x, int y)
    {
        Parameters::Inst().RenderSettings.FrameBufferDim = glm::ivec2(x, y);
    }

    void Engine::CallbackWindowSize(GLFWwindow* window, int x, int y)
    {
        Parameters::Inst().RenderSettings.WindowDim = glm::ivec2(x, y);
    }

    void Engine::CallbackFocus(GLFWwindow* window, int focused)
    {
        // handles input when several windows are created
    }

    void Engine::Run()
    {
        // Engine settings
        Parameters& params = Parameters::Inst();
        Input& input = Input::Inst();

        while (!glfwWindowShouldClose(mWindow))
        {
            // Profiler
            Parameters& params = Parameters::Inst();
            float time0 = glfwGetTime();

            // Engine update
            Update();
            Render();

            // Clears user input
            input.ClearMouseScrollMotion();
            input.ClearButtonsPressed();
            input.ClearKeysPressed();
            input.ClearMouseMotion();

            // Swap buffers and poll events
            glfwSwapBuffers(mWindow);
            glfwPollEvents();

            // Measure application time
            params.Profiler.DtApp = glfwGetTime() - time0;
            params.Profiler.Spf = 0.9 * params.Profiler.Spf + (1.0 - 0.9) * params.Profiler.DtApp;
        }

        glfwDestroyWindow(mWindow);
        glfwTerminate();
        exit(EXIT_SUCCESS);
    }

    std::shared_ptr<Viewport> Engine::AddViewport(const std::string& name)
    {
        std::shared_ptr<Viewport> v = std::make_shared<Viewport>(name);
        AddViewport(v);
        return v;
    }

    void Engine::RemoveViewport(std::shared_ptr<Viewport> v)
    {

        // Removes only if it actually exists on vector
        auto it = std::find(mViewports.begin(), mViewports.end(), v);
        if (it != mViewports.end()) mViewports.erase(it);
    }

    void Engine::AddViewport(std::shared_ptr<Viewport> v)
    {
        mViewports.push_back(v);
    }

    void Engine::LoadShader(const std::string& key, const std::vector<std::string>& files)
    {
        Shaders& shaders = Shaders::Inst();
        shaders.Load(key, files);
    }
} // namespace X3