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

		class Trapezoid : public Renderable
		{
		public:

			// Common methods
			Trapezoid(const std::string& name, const std::vector<glm::vec3>& vertices = {});
			Trapezoid() {};

			void ReDraw() override;

			void Render() override;
			void RenderUi() override;

			// Get/Set
			std::vector<glm::vec3> Vertices() { return mVertices; }

		protected:

			// Cpu checkpoint
			std::vector<glm::vec3> mVertices;

			// Render data
			std::shared_ptr<Vbo<glm::vec3>> mVbo;
			std::shared_ptr<Vbo<int>> mEbo;
			std::shared_ptr<Vao> mVao;

			// Helpers
			void BuildBoundingBox() override;
			void SetupBuffers();
		};

	} // namespace geom
} // namespace X3