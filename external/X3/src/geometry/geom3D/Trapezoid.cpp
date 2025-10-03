#include "Trapezoid.h"

namespace X3
{
	namespace geom
	{
		Trapezoid::Trapezoid(const std::string& name, const std::vector<glm::vec3>& vertices)
		{
			// Vertex order:
			// 
			// Bottom face
			// Bottom-left-back
			// Bottom-right-back
			// Bottom-right-front
			// Bottom-left-front

			// Top face
			// Top-left-back
			// Top-right-back
			// Top-right-front
			// Top-left-front

			// Check if vertices given, otherwise use default
			if (vertices.empty())
			{
				mVertices = { glm::vec3(-1,-1,-1), glm::vec3(1,-1,-1), glm::vec3(1,1,-1), glm::vec3(-1,1,-1),
				glm::vec3(-1,-1,1), glm::vec3(1,-1,1), glm::vec3(1,1,1), glm::vec3(-1,1,1) };
			}
			else mVertices = vertices;
			 
			// Setup main properties
			mPrimitive = GL_LINES;
			mName = name;

			// Build GPU buffers
			SetupBuffers();

			// Additional properties
			BuildBoundingBox();
		}

		void Trapezoid::ReDraw()
		{
			// Resets buffers
			mVao.reset();
			mEbo.reset();
			mVbo.reset();

			// Setup new data
			SetupBuffers();
		}

		void Trapezoid::Render()
		{
			mVao->Bind();
			glDrawElements(mPrimitive, 24, GL_UNSIGNED_INT, 0);
			mVao->Unbind();
		}

		void Trapezoid::RenderUi()
		{
			ImGui::Text("Name: %s", mName.c_str());
			ImGui::Text("Material: %s", mPasses[RenderPass::Forward].c_str());
		}

		void Trapezoid::BuildBoundingBox()
		{
			// Computes bounding box
			float maxX = std::numeric_limits<float>::lowest();
			float maxY = std::numeric_limits<float>::lowest();
			float maxZ = std::numeric_limits<float>::lowest();

			float minX = std::numeric_limits<float>::max();
			float minY = std::numeric_limits<float>::max();
			float minZ = std::numeric_limits<float>::max();

			// Iterate over all vertices to find the min and max points
			for (const auto& vertex : mVertices)
			{
				if (vertex.x < minX) minX = vertex.x;
				if (vertex.y < minY) minY = vertex.y;
				if (vertex.z < minZ) minZ = vertex.z;

				if (vertex.x > maxX) maxX = vertex.x;
				if (vertex.y > maxY) maxY = vertex.y;
				if (vertex.z > maxZ) maxZ = vertex.z;
			}

			mBoundingBox = XM::AABB(minX, maxX, minY, maxY, minZ, maxZ);
		}

		void Trapezoid::SetupBuffers()
		{

			std::vector<int> indices = {
				// Bottom face edges
				0, 1, 1, 2, 2, 3, 3, 0,
				// Top face edges
				4, 5, 5, 6, 6, 7, 7, 4,
				// Vertical edges
				0, 4, 1, 5, 2, 6, 3, 7
			};

			// Init render data
			mEbo = std::make_shared<Vbo<int>>(indices, GL_DYNAMIC_DRAW, GL_ELEMENT_ARRAY_BUFFER);
			mVbo = std::make_shared<Vbo<glm::vec3>>(mVertices, GL_DYNAMIC_DRAW);

			mVao = std::make_shared<Vao>(std::vector<VertexAttrib>{VertexAttrib(mVbo, 0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0)});

			mVao->BindEbo(mEbo);
		}
	} // namespace geom
} // namespace X3