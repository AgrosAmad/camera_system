#include <RendererFwd.h>

namespace X3
{
	RendererFwd::RendererFwd()
	{
		// Setup ubo's
		mUboCamera.BindBufferBase(0);
		mUboLightPoint.BindBufferBase(1);
		mUboLightDirec.BindBufferBase(2);
		mUboLightSpot.BindBufferBase(3);
		mUboRendering.BindBufferBase(5);
		
		// Setup sky
		mRenderSky = false;
	}

	void RendererFwd::Render(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera)
	{

		// Managers
		Parameters& params = Parameters::Inst();
		Shaders& shaders = Shaders::Inst();

		// Unwraps shaders
		std::map<std::string, std::shared_ptr<Shader>>& fwdShaders = shaders.FwdCache();

		// Update shared data
		UpdateUbos(camera, scene->Lights());

		// Forward rendering
		glViewport(0, 0, params.RenderSettings.WindowDim.x, params.RenderSettings.WindowDim.y);
		glClearColor(params.RenderSettings.BGColor.x, params.RenderSettings.BGColor.y, params.RenderSettings.BGColor.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		for (auto& shader : fwdShaders)
		{
			shader.second->Render();
		}

	}

	UboLight RendererFwd::LightData(std::shared_ptr<Light> l)
	{
		return UboLight();
	}

	void RendererFwd::Resize(const glm::ivec2& d)
	{
	}

	void RendererFwd::Update()
	{

	}
	void RendererFwd::UpdateUbos(std::shared_ptr<Camera> camera, std::vector<std::shared_ptr<Light>>& lights)
	{
		Parameters& params = Parameters::Inst();

		// Viewport camera
		UboCamera uboCamera = camera->UboParams(params.WindowRatio());
		mUboCamera.AddData(&uboCamera);

		// Scene lights
		int numDirec = 0, numSpot = 0, numPoint = 0;
		std::vector<UboLight> uboPoint, uboDirec, uboSpot;
		UboRendering uboRender;

		for (int i = 0; i < lights.size(); i++)
		{
			std::shared_ptr<Light> light = lights[i];

			if (light->Type() == Point)
			{
				numPoint++;
				uboPoint.push_back(light->UboParams());
			}
			else if (light->Type() == Directional)
			{
				numDirec++;
				uboDirec.push_back(light->UboParams());
			}
			else
			{
				numSpot++;
				uboSpot.push_back(light->UboParams());
			}
		}

		uboRender.NumLightDirec = numDirec;
		uboRender.NumLightPoint = numPoint;
		uboRender.NumLightSpot = numSpot;
		uboRender.Time = 0.f;

		mUboRendering.AddData(&uboRender);
		mUboLightDirec.AddData(uboDirec);
		mUboLightPoint.AddData(uboPoint);
		mUboLightSpot.AddData(uboSpot);
	}
} // namespace X3