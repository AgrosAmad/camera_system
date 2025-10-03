#include "Curve2D.h"

namespace X3
{
    namespace geom
    {
        Curve2D::Curve2D(const std::string &name, const std::shared_ptr<geom::Grid1D> &grid, std::function<float(float)> f)
        {

            // Setup general properties
            mPrimitive = GL_LINE_STRIP;
            mName = name;

            // Build GPU buffers
            SetupBuffers(grid, f);
        }

        Curve2D::Curve2D(const std::string &name, const std::shared_ptr<geom::Grid2D> &grid, std::function<float(float)> f)
        {

            // Setup general properties
            mPrimitive = GL_LINE_STRIP;
            mName = name;

            // Build GPU buffers
            SetupBuffers(grid, f);
        }

        void Curve2D::Render()
        {
            mVao->Bind();
			glDrawElements(mPrimitive, mDataSize, GL_UNSIGNED_INT, 0);
			mVao->Unbind();
        }

        void Curve2D::RenderUi()
        {
            ImGui::Text("Name: %s", mName.c_str());
			ImGui::Text("Material: %s", mPasses[RenderPass::Forward].c_str());
            ImGui::Checkbox("Render", &mVisible);
        }

        std::vector<glm::vec2> Curve2D::Points()
        {
            // Get vbo data
            glm::vec2* data = mVbo->MapBuffer(GL_READ_WRITE);

            // Create vector
            std::vector<glm::vec2> dataVec(data, data + mDataSize);

            // Unmap vbo
            mVbo->UnmapBuffer();

            return dataVec;
        }

        void Curve2D::SetupBuffers(const std::shared_ptr<geom::Grid1D> &grid, std::function<float(float)> f)
        {
            // Get x-axis info
            float xa = grid->BoundingBox().LimitsX[0];
            float xb = grid->BoundingBox().LimitsX[1];
            int nx = grid->Nx();
            float dx = (xb - xa) / (float)nx;

            // Temporal buffers
            std::vector<int> indices(nx);
            std::vector<glm::vec2> vertices(nx);

            // Generate line points
            #pragma omp parallel for
            for (int i = 0; i < nx; i++) 
            {
                // Uses function definition
                glm::vec2 point;
                point.x = xa + (i + 0.5f) * dx;
                point.y = f(point.x);

                vertices[i] = point;
                indices[i] = i;
            }

            mDataSize = nx;

            // Sends data to opengl
            mEbo = std::make_shared<Vbo<int>>(indices, GL_DYNAMIC_DRAW, GL_ELEMENT_ARRAY_BUFFER);
            mVbo = std::make_shared<Vbo<glm::vec2>>(vertices, GL_DYNAMIC_DRAW);

            mVao = std::make_shared<Vao>(std::vector<VertexAttrib>{VertexAttrib(mVbo, 0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0)});
            mVao->BindEbo(mEbo);
        }

        void Curve2D::SetupBuffers(const std::shared_ptr<geom::Grid2D> &grid, std::function<float(float)> f)
        {
            // Get x-axis info
            float xa = grid->BoundingBox().LimitsX[0];
            float xb = grid->BoundingBox().LimitsX[1];
            int nx = grid->Nx();
            float dx = (xb - xa) / (float)nx;

            // Temporal buffers
            std::vector<int> indices(nx);
            std::vector<glm::vec2> vertices(nx);

            // Generate line points
            #pragma omp parallel for
            for (int i = 0; i < nx; i++) 
            {
                // Uses function definition
                glm::vec2 point;
                point.x = xa + (i + 0.5f) * dx;
                point.y = f(point.x);

                vertices[i] = point;
                indices[i] = i;
            }

            mDataSize = nx;

            // Sends data to opengl
            mEbo = std::make_shared<Vbo<int>>(indices, GL_DYNAMIC_DRAW, GL_ELEMENT_ARRAY_BUFFER);
            mVbo = std::make_shared<Vbo<glm::vec2>>(vertices, GL_DYNAMIC_DRAW);

            mVao = std::make_shared<Vao>(std::vector<VertexAttrib>{VertexAttrib(mVbo, 0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0)});
            mVao->BindEbo(mEbo);
        }
    } // namespace geom
} // namespace X3