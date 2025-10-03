#include "Points.h"

namespace X3
{
	namespace geom
	{
		Points::Points(const std::vector<glm::vec3>& positions, const glm::vec3& color, const std::string& name)
		{

			// Set data
			mColors.assign(positions.size(), color);
			mPositions = positions;
			mPrimitive = GL_POINTS;
			mName = name;

			// Init OpenGL buffers
			SetupBuffers();

		}

		GLuint Points::GetVbo()
		{
			return mVboPos->GetID();
		}

		void Points::Render()
		{
			mVao->Bind();
			glDrawArrays(mPrimitive, 0, mPositions.size());
			mVao->Unbind();
		}

		void Points::RenderUi()
		{
			ImGui::Text("Name: %s", mName.c_str());
			ImGui::Text("Material: %s", mPasses[RenderPass::Forward].c_str());
			ImGui::End();
		}

		void Points::SetupBuffers()
		{
			// Init render data
			mVboPos = std::make_shared<Vbo<glm::vec3>>(mPositions, GL_DYNAMIC_DRAW);
			mVboColor = std::make_shared<Vbo<glm::vec3>>(mColors, GL_DYNAMIC_DRAW);

			mVao = std::make_shared<Vao>(std::vector<VertexAttrib>{
				VertexAttrib(mVboPos, 0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0),
				VertexAttrib(mVboColor, 1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0)
			});
		}
	}
}
