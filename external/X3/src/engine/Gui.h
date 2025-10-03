#pragma once

// STL
#include <functional>
#include <algorithm> 
#include <memory>
#include <map>

// GL
#include <implot.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>

#include <random>

namespace X3
{

	// Cross-def
	class Renderable;
	class Engine;
	class Scene;

	class Gui
	{
	public:

		// Constructor
		Gui(GLFWwindow* window);
		~Gui();

		// Loaders
		void UiCallback(std::function<void(std::shared_ptr<Engine>)> c) { mLoader = c; }
		std::function<void(std::shared_ptr<Engine>)> UiCallback() { return mLoader; }

		// Common methods
		void Render(Engine* engine);

	private:

		// Ui data
		std::function<void(std::shared_ptr<Engine> engine)> mLoader = nullptr;
		std::weak_ptr<Renderable> mSelectedEntity;

		ImGuiDockNodeFlags mDockspaceFlags;
		bool mOptDockingFullscreen = true;
		bool mOptPadding = false;

		// Helpers
		void SelectEntity(std::shared_ptr<Renderable> e) { mSelectedEntity = e; }
		void ActivateDocking();
		void ApplyImguiStyle();

		void RenderObjectProperties(const std::shared_ptr<Scene>& scene);
		void RenderObjectTree(const std::weak_ptr<Renderable>& object);
		void RenderProfiler();

	};
} // namespace X3