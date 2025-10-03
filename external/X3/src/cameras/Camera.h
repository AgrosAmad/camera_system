#pragma once

// STL
#include <memory>

// XE
#include <Renderable.h>
#include <Parameters.h>
#include <Input.h>

namespace X3
{
	// Types of camera
	enum CameraType { FPS, ORBIT };

	// Common camera parameters
	struct CameraParams
	{
		// Projection and view matrices
		glm::mat4 MatP, MatV;

		// Position and orientation
		glm::vec3 Pos, Dir;

		// Others
		float Fov, zNear, zFar, Vel, Boost;
		CameraType Type;
	};

	// Camera parameters for ubo
	struct UboCamera
	{
		glm::mat4 MatP;
		glm::mat4 MatV;
		glm::vec3 Pos;
		float padding1;
		glm::vec3 Dir;
		float zNear;
		float zFar;
		float padding2;
	};


	class Camera : public Renderable
	{
	public:
		// Common methods
		virtual void UpdateOrientation();
		virtual void UpdatePosition();
		virtual void Update();

		// Get/Set
		virtual CameraParams Params(const float& aspect);
		virtual UboCamera UboParams(const float& aspect);
		CameraType Type() { return mParams.Type; }

		float zNear() { return mzNear; }
		float zFar() { return mzFar; }

		virtual glm::mat4 MatP(const float& aspect) = 0;
		virtual glm::mat4 MatV() = 0;

		virtual void InputHandling() {};
		virtual void RenderUi();

	protected:

		// General params
		CameraParams mParams;

		// Inside params
		glm::vec3 mDeltaPYR = glm::vec3(0.f);
		glm::vec3 mDeltaPos = glm::vec3(0.f);
		float mFov, mzNear, mzFar, mVel, mBoost;
		CameraType mType;
		bool mCatchInput;
	};
} // namespace X3