#pragma once

// XE
#include <Camera.h>

namespace X3
{
	class CameraOrbit : public Camera
	{
	public:

		// Common methods
		CameraOrbit(const glm::vec3& target = glm::vec3(0.f, 0.f, 0.f), const float& distance = 5.f);
		void UpdateOrientation() override;
		void UpdatePosition() override;
		void InputHandling() override;

		// Get/Set
		glm::mat4 MatP(const float& aspect) override;
		glm::mat4 MatV() override;

	private:

		// Orbit params
		glm::vec3 mTarget;
		float mDistance;
	};
} // namespace X3