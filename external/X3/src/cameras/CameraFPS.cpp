#include "CameraFPS.h"

namespace X3
{
	CameraFPS::CameraFPS(const glm::vec3& pos, const glm::vec3& dir)
	{
		// Initialization
		mPosition = pos;

		mzFar = 100.f;
		mzNear = 0.1f;
		mFov = 45.f;

		mBoost = 5.f;
		mVel = 3.5f;

		mType = CameraType::FPS;
		ViewDir(dir);
	}

	void CameraFPS::InputHandling()
	{
		// Prepares data
		Parameters& params = Parameters::Inst();
		Input& input = Input::Inst();

		glm::vec3 dir = glm::vec3(0.f);
		glm::vec3 view = ViewVector();
		glm::vec3 a = glm::vec3(0.f);
		glm::vec3 up = UpVector();

		// WASD position
		if (input.KeyDown(GLFW_KEY_A)) dir += glm::cross(up, view);
		if (input.KeyDown(GLFW_KEY_D)) dir -= glm::cross(up, view);
		if (input.KeyDown(GLFW_KEY_W)) dir += view;
		if (input.KeyDown(GLFW_KEY_S)) dir -= view;

		if (input.KeyDown(GLFW_KEY_LEFT_SHIFT)) dir *= mBoost;

		mDeltaPos = mVel * dir * params.Profiler.DtApp;

		// Orientation with mouse
		if (input.ButtonDown(GLFW_MOUSE_BUTTON_MIDDLE))
		{
			mDeltaPYR = glm::vec3(-input.MouseDeltaY(), -input.MouseDeltaX(), 0.f) * params.Main.MouseSens;
		}
	}

	glm::mat4 CameraFPS::MatP(const float& aspect)
	{
		return glm::perspective(glm::radians(mFov), aspect, mzNear, mzFar);
	}

	glm::mat4 CameraFPS::MatV()
	{
		return glm::lookAt(mPosition, mPosition + ViewVector(), UpVector());
	}

}