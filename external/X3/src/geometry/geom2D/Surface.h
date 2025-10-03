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
#include <Grid2D.h>
#include <Grid3D.h>
#include <Vao.h>
#include <Vbo.h>

// Math
#include <XMath.cuh>
#include <functional>

namespace X3
{
	namespace geom
	{

		class Surface : public Renderable
		{
		public:

			// Common methods
			Surface(const std::string& name, const std::shared_ptr<geom::Grid2D>& grid, std::function<float(glm::vec2)> f);
			Surface(const std::string& name, const std::shared_ptr<geom::Grid3D>& grid, std::function<float(glm::vec2)> f);
			Surface() {};

			// Rendering
			void Render() override;
			void RenderUi() override;

			// Get/Set
			GLuint GetVbo() override { return mVbo->GetID(); }
			std::vector<glm::vec3> Points();

			// Import/Export
			void SaveObj(const std::string& filename);

		private:

			// Render data
			std::shared_ptr<Vbo<glm::vec3>> mVbo;
			std::shared_ptr<Vbo<int>> mEbo;
			std::shared_ptr<Vao> mVao;

			int mNumIndices;

			// Helpers
			void SetupBuffers(const std::shared_ptr<geom::Grid2D>& grid, std::function<float(glm::vec2)> f);
			void SetupBuffers(const std::shared_ptr<geom::Grid3D>& grid, std::function<float(glm::vec2)> f);
		};

	} // namespace geom
} // namespace X3