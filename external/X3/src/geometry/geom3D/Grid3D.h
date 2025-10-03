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

		class Grid3D : public Renderable
		{
		public:

			// Common methods
			Grid3D(const std::string& name, XM::AABB bounds, const int& nx, const int& ny, const int& nz);
			Grid3D() {};

			// Rendering
			void ReDraw() override;
			void Render() override;
			void RenderUi() override;

			// Get/Set
			glm::vec3 Ds() const { return mDs; }
			void Ds(glm::vec3& ds) { mDs = ds; }

			int Nx() const { return mNx; }
			void Nx(int nx) { mNx = nx; }

			int Ny() const { return mNy; }
			void Ny(int ny) { mNy = ny; }

			int Nz() const { return mNz; }
			void Nz(int nz) { mNy = nz; }

		protected:

			// Grid properties
			int mNx, mNy, mNz;
			glm::vec3 mDs;

			// Render data
			std::shared_ptr<Vbo<glm::vec3>> mVbo;
			std::shared_ptr<Vbo<int>> mEbo;
			std::shared_ptr<Vao> mVao;
			int mNumIndices;

			// Helpers
			void SetupBuffers();
		};

	} // namespace geom
} // namespace X3