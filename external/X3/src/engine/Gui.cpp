// XE
#include <Parameters.h>
#include <Materials.h>
#include <Engine.h>
#include <Scene.h>
#include <Input.h>
#include <Gui.h>

namespace X3
{
    Gui::Gui(GLFWwindow* window)
    {
        // Setup ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();
        ApplyImguiStyle();

        mDockspaceFlags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;

    }

    Gui::~Gui()
    {
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Gui::Render(Engine* engine)
    {
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Helper for docking
        ActivateDocking();

        // Render UI of all objects in scene
        std::shared_ptr<Scene>& scene = engine->GetScene();

        //ImGui::ShowDemoWindow();

        // Object editor
        RenderObjectProperties(scene);

        // App profiler
        RenderProfiler();

        // end docking
        ImGui::EndFrame();

        // Actual rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Gui::RenderObjectProperties(const std::shared_ptr<Scene>& scene)
    {
        // Unwrap objects and parameters
        std::vector<std::shared_ptr<Renderable>>& renderables = scene->Renderables();
        std::vector<std::shared_ptr<Light>>& lights = scene->Lights();

        Parameters& params = Parameters::Inst();
        Input& input = Input::Inst();

        // Create a window for the manager
        ImGui::Begin("Object Manager");

        // Create a single-column table
        if(ImGui::BeginTable("ObjectTable", 1))
        {
        ImGui::TableSetupColumn("Name");
        ImGui::TableHeadersRow();

        // Objects
        for (int i = 0; i < renderables.size(); ++i)
        {
            if (renderables[i]->Parent().lock() == nullptr)
            {
                ImGui::PushID(i);
                RenderObjectTree(renderables[i]);
                ImGui::PopID();
            }
        }

        // Lights
        const int numObjects = renderables.size();
        for (int i = 0; i < lights.size(); i++)
        {
            ImGui::PushID(i + numObjects);
            RenderObjectTree(lights[i]);
            ImGui::PopID();
        }

        // End object manager window
        ImGui::EndTable();
        }
        ImGui::End();

        // Create the properties window
        ImGui::Begin("Object Properties");

        // Show properties of the selected object
        if (auto selected = mSelectedEntity.lock()) selected->RenderUi();
        else ImGui::Text("Object Properties.");

        // End the properties window
        ImGui::End();
    }

    void Gui::RenderObjectTree(const std::weak_ptr<Renderable>& object)
    {
        Parameters& params = Parameters::Inst();
        auto lockedObj = object.lock();

        // Set new row, column 0
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        // We use a tree node, check if this entity has children
        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        if (lockedObj->Children().empty())
        {
            nodeFlags |= ImGuiTreeNodeFlags_Leaf;
        }

        // Check if it is selected
        if (mSelectedEntity.lock() == lockedObj)
        {
            nodeFlags |= ImGuiTreeNodeFlags_Selected;
        }

        bool nodeOpen = ImGui::TreeNodeEx((void*)&lockedObj, nodeFlags, "%s", lockedObj->Name().c_str());

        if (ImGui::IsItemClicked()) {
            mSelectedEntity = lockedObj;
        }

        if (nodeOpen)
        {
            for (auto& child : lockedObj->Children())
            {
                RenderObjectTree(child);
            }
            ImGui::TreePop();
        }

    }

    void Gui::RenderProfiler()
    {
        Textures2D& textures = Textures2D::Inst();
        Materials& materials = Materials::Inst();
        Parameters& params = Parameters::Inst();
        Shaders& shaders = Shaders::Inst();

        ImGui::Begin("Profiler");
        if (ImGui::BeginTabBar("MyTabBar"))
        {
            // Tab 1
            if (ImGui::BeginTabItem("Timing"))
            {
                ImGui::Text("FPS = %.1f", 1.f / params.Profiler.Spf);
                ImGui::Text("Time per cycle = %.5f", 1.f / params.Profiler.DtApp);
                ImGui::Text("Time per render = %.5f", 1.f / params.Profiler.DtRender);
                ImGui::EndTabItem();
            }

            // Tab 2
            if (ImGui::BeginTabItem("Resources"))
            {
                ImGui::Text("Shaders: %i", shaders.CacheSize());
                ImGui::Text("Materials: %i", materials.CacheSize());
                ImGui::Text("Textures: %i", textures.CacheSize());
                // Place your content for Tab 2 here
                ImGui::EndTabItem();
            }

            // Tab 3
            if (ImGui::BeginTabItem("Other"))
            {
                ImGui::Text("This is content in Tab 3.");
                // Place your content for Tab 3 here
                ImGui::EndTabItem();
            }

            // End the tab bar
            ImGui::EndTabBar();
        }
        ImGui::End();
    }

    void Gui::ActivateDocking()
    {
        ///////////////////////////////////////////////////
        // Next code and comments mostly from Imgui demo //
        ///////////////////////////////////////////////////

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
        if (mOptDockingFullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            mDockspaceFlags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (mDockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
        {
            window_flags |= ImGuiWindowFlags_NoBackground;
        }

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!mOptPadding) ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", NULL, window_flags);
        if (!mOptPadding) ImGui::PopStyleVar();
        if (mOptDockingFullscreen) ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), mDockspaceFlags);
        }

        ImGui::End();

    }

    void Gui::ApplyImguiStyle()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.Alpha = 1.0;
        style.WindowRounding = 3;
        style.GrabRounding = 1;
        style.GrabMinSize = 20;
        style.FrameRounding = 3;

        style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.00f, 0.40f, 0.41f, 1.00f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 1.00f, 1.00f, 0.65f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.44f, 0.80f, 0.80f, 0.18f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.44f, 0.80f, 0.80f, 0.27f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.44f, 0.81f, 0.86f, 0.66f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.18f, 0.21f, 0.73f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.27f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.29f, 0.30f, 0.71f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.44f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 0.68f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.36f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.00f, 0.65f, 0.65f, 0.46f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.01f, 1.00f, 1.00f, 0.43f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.62f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 1.00f, 0.33f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.42f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 1.00f, 0.22f);
    }
} // namespace X3