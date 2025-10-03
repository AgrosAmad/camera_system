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

// Math
#include <XMath.cuh>

namespace X3
{
	namespace geom
	{

		class Grid2D : public Renderable
		{
		public:

			// Common methods
			Grid2D(const std::string& name, XM::AABB bounds, const int& nx, const int& ny);
			Grid2D() {};

            // Rendering
			void ReDraw() override;
			void Render() override;
			void RenderUi() override;

			// Get/Set
			glm::vec2 Ds() const { return mDs; }
			void Ds(glm::vec2& ds) { mDs = ds; }

			int Nx() const { return mNx; }
			void Nx(int nx) { mNx = nx; }

			int Ny() const { return mNy; }
			void Ny(int ny) { mNy = ny; }

		protected:

            // Grid properties
            glm::vec2 mDs;
            int mNx, mNy;

			// Render data
			std::shared_ptr<Vbo<glm::vec2>> mVbo;
			std::shared_ptr<Vbo<int>> mEbo;
			std::shared_ptr<Vao> mVao;
			int mNumIndices;

			// Helpers
			void SetupBuffers();
		};

	} // namespace geom
} // namespace X3