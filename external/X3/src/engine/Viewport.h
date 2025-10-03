#pragma once

// STL
#include <memory>
#include <string>

// GL
#include <glm/glm.hpp>

namespace X3
{
	// Cross-referencing
	class Renderer;
	class Camera;
	class Scene;

	class Viewport
	{
	public:

		// Constructor
		Viewport(const std::string& name);

		// Common methods
		void Render(std::shared_ptr<Scene> scene);
		void Resize(const glm::ivec2& size);
		void Update();

		// Getters/Setters
		bool HasCameraAssigned() { return (mCamera.expired()) ? false : mCamera.lock() != mViewportCamera; }
		void AssignViewportCamera() { mCamera = mViewportCamera; }
		void SetCamera(std::shared_ptr<Camera> c) { mCamera = c; }

		std::shared_ptr<Renderer> GetRenderer() { return mRenderer; }

		void IsHovered(const bool& b) { mHovered = b; }
		bool IsHovered() { return mHovered; }

		void FullScreen(const bool& b) { mFullScreen = b; }
		bool& FullScreen() { return mFullScreen; }

		void Size(const glm::ivec2& s) { mSize = s; }
		glm::ivec2 Size() { return mSize; }

		std::string Name() { return mName; }
	private:

		std::shared_ptr<Camera> mViewportCamera;
		std::shared_ptr<Renderer> mRenderer;
		std::weak_ptr<Camera> mCamera;
		std::string mName;
		bool mFullScreen;
		glm::ivec2 mSize;
		bool mHovered;
	};
} // namespace X3