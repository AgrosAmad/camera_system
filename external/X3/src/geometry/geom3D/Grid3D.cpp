#include <Grid3D.h>

namespace X3
{
    namespace geom
    {
        Grid3D::Grid3D(const std::string& name, XM::AABB bounds, const int& nx, const int& ny, const int& nz)
        {
            // Setup grid limits
            mBoundingBox = bounds;

            // Setup additional properties
            mPrimitive = GL_LINES;
            mName = name;
            mNx = nx;
            mNy = ny;
            mNz = nz;

            // Build GPU buffers
            SetupBuffers();
        }

        void Grid3D::ReDraw()
        {
        }

        void Grid3D::Render()
        {
            mVao->Bind();
            glDrawElements(mPrimitive, mNumIndices, GL_UNSIGNED_INT, 0);
            mVao->Unbind();
        }

        void Grid3D::RenderUi()
        {
            ImGui::Text("Name: %s", mName.c_str());
            ImGui::Text("Material: %s", mPasses[RenderPass::Forward].c_str());
            ImGui::Checkbox("Render", &mVisible);
        }

        void Grid3D::SetupBuffers()
        {

            // Compute spacing
            mDs.x = (mBoundingBox.LimitsX[1] - mBoundingBox.LimitsX[0]) / mNx;
            mDs.y = (mBoundingBox.LimitsY[1] - mBoundingBox.LimitsY[0]) / mNy;
            mDs.z = (mBoundingBox.LimitsZ[1] - mBoundingBox.LimitsZ[0]) / mNz;

            // Grid dimensions
            int colsX = mNx + 1;
            int colsY = mNy + 1;
            int colsZ = mNz + 1;

            // Temporal buffers
            std::vector<glm::vec3> vertices;
            std::vector<int> indices;

            // Lambda to flatten 3D index
            auto idx = [&](int i, int j, int k) {
                return i + j * colsX + k * colsX * colsY;
                };

            // Generate vertices and edges
            for (int k = 0; k <= mNz; ++k) 
            {
                for (int j = 0; j <= mNy; ++j) 
                {
                    for (int i = 0; i <= mNx; ++i)
                    {
                        float x = mBoundingBox.LimitsX[0] + i * mDs.x;
                        float y = mBoundingBox.LimitsY[0] + j * mDs.y;
                        float z = mBoundingBox.LimitsZ[0] + k * mDs.z;
                        vertices.emplace_back(x, y, z);

                        // Edges in X direction
                        if (i < mNx) {
                            indices.push_back(idx(i, j, k));
                            indices.push_back(idx(i + 1, j, k));
                        }

                        // Edges in Y direction
                        if (j < mNy) {
                            indices.push_back(idx(i, j, k));
                            indices.push_back(idx(i, j + 1, k));
                        }

                        // Edges in Z direction
                        if (k < mNz) {
                            indices.push_back(idx(i, j, k));
                            indices.push_back(idx(i, j, k + 1));
                        }
                    }
                }
            }


            // Sends data to opengl
            mEbo = std::make_shared<Vbo<int>>(indices, GL_DYNAMIC_DRAW, GL_ELEMENT_ARRAY_BUFFER);
            mVbo = std::make_shared<Vbo<glm::vec3>>(vertices, GL_DYNAMIC_DRAW);

            mVao = std::make_shared<Vao>(std::vector<VertexAttrib>{VertexAttrib(mVbo, 0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0)});
            mVao->BindEbo(mEbo);

            mNumIndices = indices.size();
        }
    } // namespace geom
} // namespace X3