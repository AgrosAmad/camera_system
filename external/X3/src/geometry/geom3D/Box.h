#pragma once

// GL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// X3
#include <Renderable.h>
#include <Materials.h>
#include <Textures.h>
#include <Vao.h>
#include <Vbo.h>

namespace X3
{
	namespace geom
	{

		class Box : public Renderable
		{
		public:

			// Common methods
			Box(const std::string& name, float minX, float maxX, float minY, float maxY, float minZ, float maxZ);
			Box(const std::string& name, const float& min = -2, const float& max = 2);
			Box(XM::AABB bounds, const std::string& name = "box");
			Box() {};

			void ReDraw() override;

			void Render() override;
			void RenderUi() override;

		protected:

			// Render data
			std::shared_ptr<Vbo<glm::vec3>> mVbo;
			std::shared_ptr<Vbo<int>> mEbo;
			std::shared_ptr<Vao> mVao;

			// Helpers
			void SetupBuffers();
		};

	} // namespace geom
} // namespace X3