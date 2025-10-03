#include "Box.h"

namespace X3
{
	namespace geom
	{
		Box::Box(const std::string& name, const float& min, const float& max)
		{
			// Setup box limits
			mBoundingBox = XM::AABB(min, max);

			// Setup additional properties
			mPrimitive = GL_LINES;
			mName = name;

			// Build GPU buffers
			SetupBuffers();
		}
		Box::Box(const std::string& name, float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
		{

			// Setup box limits
			mBoundingBox = XM::AABB(minX, maxX, minY, maxY, minZ, maxZ);

			// Setup additional properties
			mPrimitive = GL_LINES;
			mName = name;

			// Build GPU buffers
			SetupBuffers();
		}

		Box::Box(XM::AABB bounds, const std::string& name)
		{
			// Setup box limits
			mBoundingBox = bounds;

			// Setup additional properties
			mPrimitive = GL_LINES;
			mName = name;

			// Build GPU buffers
			SetupBuffers();
		}

		void Box::ReDraw()
		{
			// Resets buffers
			mVao.reset();
			mEbo.reset();
			mVbo.reset();

			// Setup new data
			SetupBuffers();
		}

		void Box::Render()
		{
			mVao->Bind();
			glDrawElements(mPrimitive, 24, GL_UNSIGNED_INT, 0);
			mVao->Unbind();
		}

		void Box::RenderUi()
		{
			ImGui::Text("Name: %s", mName.c_str());
			ImGui::Text("Material: %s", mPasses[RenderPass::Forward].c_str());
		}

		void Box::SetupBuffers()
		{
			// Builds data before sending to gpu
			std::vector<glm::vec3> vertices;

			// Bottom face
			vertices.emplace_back(mBoundingBox.LimitsX[0], mBoundingBox.LimitsY[0], mBoundingBox.LimitsZ[0]); // Bottom-left-back
			vertices.emplace_back(mBoundingBox.LimitsX[1], mBoundingBox.LimitsY[0], mBoundingBox.LimitsZ[0]); // Bottom-right-back
			vertices.emplace_back(mBoundingBox.LimitsX[1], mBoundingBox.LimitsY[0], mBoundingBox.LimitsZ[1]); // Bottom-right-front
			vertices.emplace_back(mBoundingBox.LimitsX[0], mBoundingBox.LimitsY[0], mBoundingBox.LimitsZ[1]); // Bottom-left-front

			// Top face
			vertices.emplace_back(mBoundingBox.LimitsX[0], mBoundingBox.LimitsY[1], mBoundingBox.LimitsZ[0]); // Top-left-back
			vertices.emplace_back(mBoundingBox.LimitsX[1], mBoundingBox.LimitsY[1], mBoundingBox.LimitsZ[0]); // Top-right-back
			vertices.emplace_back(mBoundingBox.LimitsX[1], mBoundingBox.LimitsY[1], mBoundingBox.LimitsZ[1]); // Top-right-front
			vertices.emplace_back(mBoundingBox.LimitsX[0], mBoundingBox.LimitsY[1], mBoundingBox.LimitsZ[1]); // Top-left-front

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
			mVbo = std::make_shared<Vbo<glm::vec3>>(vertices, GL_DYNAMIC_DRAW);

			mVao = std::make_shared<Vao>(std::vector<VertexAttrib>{VertexAttrib(mVbo, 0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0)});

			mVao->BindEbo(mEbo);
		}
	} // namespace geom
} // namespace X3