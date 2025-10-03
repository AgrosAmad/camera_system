#pragma once

// XE
#include <Camera.h>

namespace X3
{
	class CameraFPS : public Camera
	{
	public:

		// Common methods
		CameraFPS(const glm::vec3& pos = glm::vec3(0.f, 0.f, 3.f), const glm::vec3& dir = glm::vec3(0.f, 0.f, -1.f));
		void InputHandling() override;

		// Get/Set
		glm::mat4 MatP(const float& aspect) override;
		glm::mat4 MatV() override;
	};
} // namespace X3