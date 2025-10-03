#include <Grid1D.h>

namespace X3
{
    namespace geom
    {
        Grid1D::Grid1D(const std::string &name, const float &xa, const float &xb, const int &nx)
        {
            // Setup grid limits
            mBoundingBox = XM::AABB(xa, xb, 0.f, 0.f, 0.f, 0.f);
            mNx = nx;

            // Setup additional properties
            mPrimitive = GL_LINE_STRIP;
            mName = name;

            // Build GPU buffers
            SetupBuffers();
        }

        void Grid1D::ReDraw()
        {
        }

        void Grid1D::Render()
        {
            mVao->Bind();
            glDrawElements(mPrimitive, mNx + 1, GL_UNSIGNED_INT, 0);
			mVao->Unbind();
        }

        void Grid1D::RenderUi()
        {
            ImGui::Text("Name: %s", mName.c_str());
			ImGui::Text("Material: %s", mPasses[RenderPass::Forward].c_str());
            ImGui::Checkbox("Render", &mVisible);
        }

        void Grid1D::SetupBuffers()
        {
            // Compute spacing
            float dx = (mBoundingBox.LimitsX[1] - mBoundingBox.LimitsX[0]) / mNx;

            // Temporal buffers
            std::vector<int> indices(mNx + 1);
            std::vector<float> vertices(mNx + 1);

            // Generate edges of the grid (lines)
            #pragma omp parallel for
            for (int i = 0; i <= mNx; i++) 
            {
                vertices[i] = mBoundingBox.LimitsX[0] + i * dx;
                indices[i] = i;
            }

            // Sends data to opengl
            mEbo = std::make_shared<Vbo<int>>(indices, GL_DYNAMIC_DRAW, GL_ELEMENT_ARRAY_BUFFER);
            mVbo = std::make_shared<Vbo<float>>(vertices, GL_DYNAMIC_DRAW);

            mVao = std::make_shared<Vao>(std::vector<VertexAttrib>{VertexAttrib(mVbo, 0, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void *)0)});
            mVao->BindEbo(mEbo);
        }

    } // namespace geom
} // namespace X3