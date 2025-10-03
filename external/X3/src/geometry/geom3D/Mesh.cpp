#include <Mesh.h>

namespace X3
{
	namespace geom
	{
		Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<int>& indices, const std::string& name)
		{

			// Set data
			mPrimitive = GL_TRIANGLES;
			mVertices = vertices;
			mIndices = indices;
			mName = name;

			// Init mesh
			SetupGeometry();
			SetupBuffers();

		}

		std::vector<glm::vec3> Mesh::VertexPositions()
		{
			int size = mVertices.size();
			std::vector<glm::vec3> vertices(size);

			#pragma omp parallel for
			for (int i = 0; i < size; i++)
			{
				vertices[i] = mVertices[i].Position;
			}

			return vertices;
		}

		std::vector<Triangle> Mesh::Triangles()
		{
			// Fill triangle array based on mesh indices
			int size = mIndices.size() / 3;
			std::vector<Triangle> triangles(size);

			#pragma omp parallel for
			for (int i = 0; i < size; i++)
			{
				Triangle& triangle = triangles[i];

				// Get vertices
				triangle.Vertices[0] = mVertices[mIndices[3 * i + 0]].Position;
				triangle.Vertices[1] = mVertices[mIndices[3 * i + 1]].Position;
				triangle.Vertices[2] = mVertices[mIndices[3 * i + 2]].Position;

				// Compute normal
				glm::vec3 edge1 = triangle.Vertices[1] - triangle.Vertices[0];
				glm::vec3 edge2 = triangle.Vertices[2] - triangle.Vertices[0];

				// Compute the cross product of the edge vectors
				glm::vec3 normal = glm::cross(edge1, edge2);

				// Normalize the normal vector
				triangle.Normal = glm::normalize(normal);

				// Compute center and idx
				triangle.Center = (1.f / 3.f) * (triangle.Vertices[0] + triangle.Vertices[1] + triangle.Vertices[2]);
				triangle.Idx = i;
			}

			return triangles;
		}

		void Mesh::Render()
		{
			mVao->Bind();
			glDrawElements(mPrimitive, mIndices.size(), GL_UNSIGNED_INT, 0);
			mVao->Unbind();
		}

		void Mesh::RenderUi()
		{
			ImGui::Text("Name: %s", mName.c_str());
			ImGui::Text("Material: %s", mPasses[RenderPass::Forward].c_str());
			ImGui::DragFloat3("Position", &mPosition.x, 0.2f, -20.0f, 20.0f, "%.5f");
			ImGui::DragFloat3("PYR", &mPYR.x, 0.02f, 0.f, 2 * 3.14159f, "%.5f");
			ImGui::InputFloat3("Scale", &mScale.x);
		}

		void Mesh::SetupGeometry()
		{
			// Computes geometry's center
			mCenter = glm::vec3(0.f);
			for (int i = 0; i < mVertices.size(); i++)
			{
				mCenter += mVertices[i].Position;
			}
			mCenter /= mVertices.size();

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
				if (vertex.Position.x < minX) minX = vertex.Position.x;
				if (vertex.Position.y < minY) minY = vertex.Position.y;
				if (vertex.Position.z < minZ) minZ = vertex.Position.z;

				if (vertex.Position.x > maxX) maxX = vertex.Position.x;
				if (vertex.Position.y > maxY) maxY = vertex.Position.y;
				if (vertex.Position.z > maxZ) maxZ = vertex.Position.z;
			}

			mBoundingBox = XM::AABB(minX, maxX, minY, maxY, minZ, maxZ);

		}

		void Mesh::SetupBuffers()
		{
			// Init render data
			mEbo = std::make_shared<Vbo<int>>(mIndices, GL_STATIC_DRAW, GL_ELEMENT_ARRAY_BUFFER);
			mVbo = std::make_shared<Vbo<Vertex>>(mVertices);

			mVao = std::make_shared<Vao>(std::vector<VertexAttrib>{
				VertexAttrib(mVbo, 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0),
				VertexAttrib(mVbo, 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Normal))),
				VertexAttrib(mVbo, 2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, TexCoord)))
			});

			mVao->BindEbo(mEbo);
		}
	} // namespace geom
} // namespace X3
