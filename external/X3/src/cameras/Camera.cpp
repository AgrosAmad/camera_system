#include <Camera.h>

namespace X3
{
	void Camera::UpdateOrientation()
	{
		mPYR += mDeltaPYR;

		mDeltaPYR = glm::vec3(0.f);
	}

	void Camera::UpdatePosition()
	{
		mPosition += mDeltaPos;

		mDeltaPos = glm::vec3(0.f);
	}

	void Camera::Update()
	{
		UpdateOrientation();
		UpdatePosition();
	}

	CameraParams Camera::Params(const float& aspect)
	{
		CameraParams params;

		params.MatP = MatP(aspect);
		params.Dir = ViewVector();
		params.Pos = mPosition;
		params.MatV = MatV();

		params.zNear = mzNear;
		params.Boost = mBoost;
		params.zFar = mzFar;
		params.Type = mType;
		params.Fov = mFov;
		params.Vel = mVel;

		return params;
	}

	UboCamera Camera::UboParams(const float& aspect)
	{
		UboCamera params;
		params.MatP = MatP(aspect);
		params.Dir = ViewVector();
		params.Pos = mPosition;
		params.MatV = MatV();

		params.zNear = mzNear;
		params.zFar = mzFar;

		params.padding1 = 0.f;

		return params;
	}

	void Camera::RenderUi()
	{
		ImGui::Begin("Camera");
		ImGui::SliderFloat3("XYZ", &mPosition.x, -10.0f, 10.0f, "%.5f");
		ImGui::SliderFloat3("PYR", &mPYR.x, -4.f, 4.f, "%.5f");
		ImGui::End();
	}
} // namespace X3