#pragma once

// STL
#include <vector>
#include <memory>

// X3
#include <Renderable.h>
#include <CameraFPS.h>

namespace X3
{
	// Cross-definition
	class Texture2D;
	class Fbo;

	// Ubo parameters
	struct UboLight
	{
		glm::vec3 Position;
		float padding1;
		glm::vec3 ViewDir;
		float padding2;
		glm::vec3 Ambient;
		float padding3;
		glm::vec3 Diffuse;
		float padding4;
		glm::vec3 Specular;
		float Intensity;
		glm::vec3 Attenuation;
		float padding5;
		glm::vec2 ConeAngleCos;
		float padding6[2];
	};

	// Light structs
	enum LightType { Directional, Point, Spotlight };
	enum LightRender { None, Cube, Sphere };

	class Light : public Renderable
	{
	public:
		// Common methods
		Light(const std::string& name, const LightType& t = Point, const glm::vec3& c = glm::vec3(1.f), const glm::vec3& p = glm::vec3(0.f, 1.f, 0.f), const glm::vec3& dir = glm::vec3(0, -1, -1), float intensity = 0.5f, LightRender renderType = Cube, bool hasShadows = false);
		void Render() override;
		void RenderUi();
		void Update();

		// Get/Set
		UboLight UboParams();

		void Attenuation(const glm::vec3& attenuation) { mAttenuation = attenuation; }
		glm::vec3 Attenuation() { return mAttenuation; }

		glm::vec2 ConeAngleCos() { return glm::vec2(cosf(glm::radians(mConeAngle.x)), cosf(glm::radians(mConeAngle.y))); }
		void ConeAngle(const glm::vec2& angles) { mConeAngle = angles; }
		glm::vec2 ConeAngle() { return mConeAngle; }

		void ViewDir(const glm::vec3& dir) { mViewDir = dir; }
		glm::vec3 ViewDir() { return mViewDir; }

		void Color(const glm::vec3& c) { mColor = c; }
		void Type(const LightType& t) { mType = t; }

		float Intensity() { return mIntensity; }
		glm::vec3 Color() { return mColor; }
		LightType Type() { return mType; }

	private:

		// General parameters
		glm::vec3 mAttenuation;
		glm::vec3 mViewDir;
		glm::vec2 mConeAngle;
		glm::vec3 mColor;
		float mIntensity;
		LightType mType;

		// Render data
		LightRender mRenderType;
		std::shared_ptr<Vbo<float>> mVertices;
		std::shared_ptr<Vao> mVao;

		std::shared_ptr<CameraFPS> mCam;

		// TODO: Shadow map
		bool mHasShadows;
	};
} // namespace X3