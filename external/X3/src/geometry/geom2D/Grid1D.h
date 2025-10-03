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

		class Grid1D : public Renderable
		{
		public:

			// Common methods
			Grid1D(const std::string& name, const float& xa, const float& xb, const int& nx);
			Grid1D() {};

            // Rendering
			void ReDraw() override;
			void Render() override;
			void RenderUi() override;

			// Get/Set
			int Nx() const { return mNx; }
			void Nx(int nx) { mNx = nx; }

		protected:

            // Grid properties
            int mNx;

			// Render data
			std::shared_ptr<Vbo<float>> mVbo;
			std::shared_ptr<Vbo<int>> mEbo;
			std::shared_ptr<Vao> mVao;

			// Helpers
			void SetupBuffers();
		};

	} // namespace geom
} // namespace X3