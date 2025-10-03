#include "Points2D.h"

namespace X3
{
	namespace geom
	{
		Points2D::Points2D(const std::vector<glm::vec2>& positions, const glm::vec3& color, const std::string& name)
		{

			// Set data
			mDataSize = positions.size();
			mPrimitive = GL_POINTS;
			mName = name;

			// Init OpenGL buffers
			SetupBuffers(positions, color);

		}

		void Points2D::Render()
		{
			mVao->Bind();
			glDrawArrays(mPrimitive, 0, mDataSize);
			mVao->Unbind();
		}

		void Points2D::RenderUi()
		{
			ImGui::Text("Name: %s", mName.c_str());
			ImGui::Text("Material: %s", mPasses[RenderPass::Forward].c_str());
			ImGui::Checkbox("Render", &mVisible);
		}

		void Points2D::SetupBuffers(const std::vector<glm::vec2>& positions, const glm::vec3& color)
		{
			// Set equal color for all points
			std::vector<glm::vec3> colors(mDataSize, color);

			// Init render data
			mVboPos = std::make_shared<Vbo<glm::vec2>>(positions, GL_DYNAMIC_DRAW);
			mVboColor = std::make_shared<Vbo<glm::vec3>>(colors, GL_DYNAMIC_DRAW);

			mVao = std::make_shared<Vao>(std::vector<VertexAttrib>{
				VertexAttrib(mVboPos, 0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0),
				VertexAttrib(mVboColor, 1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0)
			});
		}
	}
}
