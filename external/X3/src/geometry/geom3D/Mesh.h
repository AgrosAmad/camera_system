#pragma once

// GL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// X3
#include <MeshLoader.h>
#include <Renderable.h>
#include <Materials.h>
#include <Textures.h>
#include <Vao.h>
#include <Vbo.h>

namespace X3
{
	namespace geom
	{

		// Containers
		struct Vertex
		{
			glm::vec3 Position;
			glm::vec3 Normal;
			glm::vec2 TexCoord;
		};

		struct Triangle
		{
			glm::vec3 Vertices[3];
			glm::vec3 Center;
			glm::vec3 Normal;
			int Idx = -1;
		};

		class Mesh : public Renderable
		{
		public:

			// Common methods
			Mesh(const std::vector<Vertex>& vertices, const std::vector<int>& indices, const std::string& name = "mesh");
			Mesh() {};

			void Render() override;
			void RenderUi() override;

			// Get/Set
			std::vector<Vertex> Vertices() { return mVertices; }
			std::vector<int> Indices() { return mIndices; }
			std::vector<glm::vec3> VertexPositions();
			std::vector<Triangle> Triangles();

		protected:

			// Mesh data
			std::vector<Vertex> mVertices;
			std::vector<int> mIndices;

			// Render data
			std::shared_ptr<Vbo<Vertex>> mVbo;
			std::shared_ptr<Vbo<int>> mEbo;
			std::shared_ptr<Vao> mVao;

			// Helpers
			void SetupGeometry();
			void SetupBuffers();
		};

	} // namespace geom
} // namespace X3