#include <Grid2D.h>

namespace X3
{
    namespace geom
    {
        Grid2D::Grid2D(const std::string& name, XM::AABB bounds, const int& nx, const int& ny)
        {
            // Setup grid limits
            mBoundingBox = bounds;

            // Setup additional properties
            mPrimitive = GL_LINES;
            mName = name;
            mNx = nx;
            mNy = ny;

            // Build GPU buffers
            SetupBuffers();
        }

        void Grid2D::ReDraw()
        {
        }

        void Grid2D::Render()
        {
            mVao->Bind();
			glDrawElements(mPrimitive, mNumIndices, GL_UNSIGNED_INT, 0);
			mVao->Unbind();
        }

        void Grid2D::RenderUi()
        {
            ImGui::Text("Name: %s", mName.c_str());
			ImGui::Text("Material: %s", mPasses[RenderPass::Forward].c_str());
            ImGui::Checkbox("Render", &mVisible);
        }

        void Grid2D::SetupBuffers()
        {

            // Compute spacing
            mDs.x = (mBoundingBox.LimitsX[1] - mBoundingBox.LimitsX[0]) / mNx;
            mDs.y = (mBoundingBox.LimitsY[1] - mBoundingBox.LimitsY[0]) / mNy;

            // Total number of vertices
            int cols = mNx + 1;
            int rows = mNy + 1;

            // Temporal buffers
            std::vector<int>  indices;
            std::vector<glm::vec2> vertices;

            // Generate edges of the grid (lines)
            for (int i = 0; i <= mNy; i++) 
            {
                for (int j = 0; j <= mNx; j++) 
                {
                    float x = mBoundingBox.LimitsX[0] + j * mDs.x;
                    float y = mBoundingBox.LimitsY[0] + i * mDs.y;
                    glm::vec2 pos = glm::vec2(x, y);
                    vertices.push_back(pos);
                    
                    // Add edges connecting the adjacent vertices horizontally and vertically
                    if (j < mNx) 
                    {
                        // Horizontal edges (between vertices in the same row)
                        indices.push_back(i * cols + j);
                        indices.push_back(i * cols + (j + 1));
                    }
                    if (i < mNy) 
                    {
                        // Vertical edges (between vertices in the same column)
                        indices.push_back(i * cols + j);
                        indices.push_back((i + 1) * cols + j);
                    }
                }
            }

            // Sends data to opengl
			mEbo = std::make_shared<Vbo<int>>(indices, GL_DYNAMIC_DRAW, GL_ELEMENT_ARRAY_BUFFER);
			mVbo = std::make_shared<Vbo<glm::vec2>>(vertices, GL_DYNAMIC_DRAW);

            mVao = std::make_shared<Vao>(std::vector<VertexAttrib>{VertexAttrib(mVbo, 0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0)});
            mVao->BindEbo(mEbo);

            mNumIndices = indices.size();
        }
    } // namespace geom
} // namespace X3