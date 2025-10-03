#include "Viewport.h"

// X3
#include <RendererFwd.h>
#include <CameraOrbit.h>
#include <Parameters.h>
#include <CameraFPS.h>
#include <Input.h>
#include <Scene.h>

namespace X3
{
	Viewport::Viewport(const std::string& name)
	{
		// General parameters
		mSize = glm::ivec2(640, 480);
		mFullScreen = false;
		mHovered = false;
		mName = name;

		// By default we use forward rendering and an ORBIT camera
		mViewportCamera = std::make_shared<CameraOrbit>();
		mRenderer = std::make_shared<RendererFwd>();
	}

	void Viewport::Render(std::shared_ptr<Scene> scene)
	{
		if (mRenderer && !mCamera.expired())
		{
			mRenderer->Render(scene, mCamera.lock());
		}
	}

	void Viewport::Update()
	{
		// If no camera set, use default
		if (mCamera.expired())
		{
			AssignViewportCamera();
		}

		// Handles input
		if (mFullScreen || (!mFullScreen && mHovered))
		{
			mCamera.lock()->InputHandling();
			mCamera.lock()->Update();
		}

		// Updates renderer
		if (mRenderer)
		{
			mRenderer->Update();
		}
	}
} // namespace X3