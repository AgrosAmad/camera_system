#include "CameraOrbit.h"

namespace X3
{
	CameraOrbit::CameraOrbit(const glm::vec3& target, const float& distance)
	{
		// Orbit setup
		mPYR = glm::vec3(glm::radians(0.f), glm::radians(0.f), 0.f);
		mDistance = distance;
		mTarget = target;

		// Position from orbit
		mPosition = mTarget + Orientation() * glm::vec3(0.f, 0.f, mDistance);

		// General
		mzFar = 100.f;
		mzNear = 0.1f;
		mFov = 45.f;

		// Motion
		mBoost = 5.f;
		mVel = 3.5f;

		mType = CameraType::ORBIT;
	}

	void CameraOrbit::UpdateOrientation()
	{
		mPYR += mDeltaPYR;
		mDeltaPYR = glm::vec3(0.f);

		// Clamp pitch to avoid flipping
		mPYR.x = glm::clamp(mPYR.x, glm::radians(-89.f), glm::radians(89.f));
	}

	void CameraOrbit::UpdatePosition()
	{
		mTarget += mDeltaPos;
		mPosition = mTarget + Orientation() * glm::vec3(0.f, 0.f, mDistance);
		mDeltaPos = glm::vec3(0.f);
	}

	void CameraOrbit::InputHandling()
	{
		// Prepares data
		Parameters& params = Parameters::Inst();
		Input& input = Input::Inst();

		// Orbiting with right mouse button
		if (input.ButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
		{
			glm::vec3 delta = glm::vec3(-input.MouseDeltaY(), -input.MouseDeltaX(), 0.f) * params.Main.MouseSens;
			mDeltaPYR = delta;
		}

		// Zoom with scroll wheel
		mDistance -= input.MouseScrollY() * params.Main.ScrollSens;
		mDistance = glm::max(mDistance, 0.1f); // Prevent negative or zero distance

		// Panning with middle mouse button
		if (input.ButtonDown(GLFW_MOUSE_BUTTON_MIDDLE))
		{
			glm::vec3 up = UpVector();
			glm::vec3 right = glm::cross(up, ViewVector());

			mDeltaPos += float(input.MouseDeltaX()) * right * params.Main.PanningSens;
			mDeltaPos += up * float(input.MouseDeltaY()) * params.Main.PanningSens;
		}
	}

	glm::mat4 CameraOrbit::MatP(const float& aspect)
	{
		return glm::perspective(glm::radians(mFov), aspect, mzNear, mzFar);
	}

	glm::mat4 CameraOrbit::MatV()
	{
		return glm::lookAt(mPosition, mTarget, UpVector());
	}
}
