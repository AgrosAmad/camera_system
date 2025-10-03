#pragma once

// STL
#include <map>

// X3
#include <Parameters.h>
#include <Renderer.h>
#include <Camera.h>
#include <Scene.h>
#include <Ubo.h>

namespace X3
{
	class RendererFwd : public Renderer
	{
	public:

		// Common methods
		RendererFwd();
		void Render(std::shared_ptr<Scene> scene, std::shared_ptr<Camera>) override;
		virtual UboLight LightData(std::shared_ptr<Light> l) override;
		void Resize(const glm::ivec2& d) override;
		void Update() override;
	private:
		// Shared data
		Ubo<UboRendering> mUboRendering;
		Ubo<UboLight> mUboLightPoint;
		Ubo<UboLight> mUboLightDirec;
		Ubo<UboLight> mUboLightSpot;
		Ubo<UboCamera> mUboCamera;

		// Misc
		bool mRenderSky;

		// Helpers
		void UpdateUbos(std::shared_ptr<Camera> camera, std::vector<std::shared_ptr<Light>>& lights);
	};
} // namespace X3