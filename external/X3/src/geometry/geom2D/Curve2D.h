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
#include <Grid1D.h>
#include <Grid2D.h>
#include <Vao.h>
#include <Vbo.h>

// Math
#include <XMath.cuh>

// STL
#include <functional>

namespace X3
{
	namespace geom
	{

		// Curve types
		enum class CurveType
		{
			LINE,
			UNIFORM,
			SOLITON,
			SINE
		};

		struct CurveParams
		{
			CurveType InitShape = CurveType::UNIFORM;
            float Height = 4.f;
		};

		class Curve2D : public Renderable
		{
		public:

			// Common methods
			Curve2D(const std::string& name, const std::shared_ptr<geom::Grid1D> &grid, std::function<float(float)> f);
			Curve2D(const std::string& name, const std::shared_ptr<geom::Grid2D> &grid, std::function<float(float)> f);
			Curve2D() {};

            // Rendering
			void Render() override;
			void RenderUi() override;

			// Get/Set
			GLuint GetVbo() override { return mVbo->GetID(); }
			std::vector<glm::vec2> Points();

		private:
			
			// Render data
			std::shared_ptr<Vbo<glm::vec2>> mVbo;
			std::shared_ptr<Vbo<int>> mEbo;
			std::shared_ptr<Vao> mVao;

			// Helpers
			void SetupBuffers(const std::shared_ptr<geom::Grid1D> &grid, std::function<float(float)> f);
			void SetupBuffers(const std::shared_ptr<geom::Grid2D> &grid, std::function<float(float)> f);
		};

	} // namespace geom
} // namespace X3