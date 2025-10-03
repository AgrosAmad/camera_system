#pragma once

// STL
#include <sstream>
#include <vector>
#include <memory>
#include <string>
#include <chrono>
#include <map>

// GL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// X3
#include <Materials.h>
#include <Camera.h>
#include <Light.h>
#include <Scene.h>

namespace X3
{
	// UBOs

	// Shadow map
	struct UboShadowMap
	{
		glm::vec2 Properties;
		glm::vec2 zNearFar;
		glm::vec2 EsmExp;
		GLuint64 Texture;
		glm::mat4 MatL;
	};

	// General info
	struct UboRendering
	{
		int NumLightDirec;
		int NumLightPoint;
		int NumLightSpot;
		float Time;
	};

	class Renderer
	{
	public:

		// Common methods
		virtual void Render(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera) {};
		virtual void RenderToFBO(const bool& b) {}
		virtual void Resize(const glm::ivec2& d) {};
		virtual void Update() {};
		virtual void RenderUI() {};

		// Ubo
		//virtual UboShadowMap ShadowData(std::shared_ptr<Light> light) = 0;
		virtual UboLight LightData(std::shared_ptr<Light> light) = 0;

		//virtual glm::ivec2 FBODim();
		//virtual GLuint FBOColor();
	};
} // namespace X3