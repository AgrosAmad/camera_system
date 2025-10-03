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

		class Points : public Renderable
		{
		public:

			// Common methods
			Points(const std::vector<glm::vec3>& positions, const glm::vec3& color = glm::vec3(0.f, 0.f, 1.f), const std::string& name = "points");
			Points() {};

			void Render() override;
			void RenderUi() override;

			int NumPoints() const { return static_cast<int>(mPositions.size()); }

			GLuint GetVbo() override;

		protected:

			// Points data
			std::vector<glm::vec3> mPositions;
			std::vector<glm::vec3> mColors;

			// Render data
			std::shared_ptr<Vbo<glm::vec3>> mVboPos, mVboColor;
			std::shared_ptr<Vao> mVao;

			// Helpers
			void SetupBuffers();
		};

	} // namespace geom
} // namespace X3