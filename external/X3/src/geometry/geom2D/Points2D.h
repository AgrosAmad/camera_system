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

		class Points2D : public Renderable
		{
		public:

			// Common methods
			Points2D(const std::vector<glm::vec2>& positions, const glm::vec3& color = glm::vec3(0.f, 0.f, 1.f), const std::string& name = "points");
			Points2D() {};

			void Render() override;
			void RenderUi() override;

		protected:

			// Render data
            std::shared_ptr<Vbo<glm::vec2>> mVboPos;
			std::shared_ptr<Vbo<glm::vec3>> mVboColor;
			std::shared_ptr<Vao> mVao;

			// Helpers
			void SetupBuffers(const std::vector<glm::vec2>& positions, const glm::vec3& color);
		};

	} // namespace geom
} // namespace X3