#include <Surface.h>

namespace X3
{
    namespace geom
    {
        Surface::Surface(const std::string& name, const std::shared_ptr<geom::Grid2D>& grid, std::function<float(glm::vec2)> f)
        {

            // Setup general properties
            mPrimitive = GL_TRIANGLES;
            mName = name;

            // Build GPU buffers
            SetupBuffers(grid, f);
        }


        Surface::Surface(const std::string& name, const std::shared_ptr<geom::Grid3D>& grid, std::function<float(glm::vec2)> f)
        {

            // Setup general properties
            mPrimitive = GL_TRIANGLES;
            mName = name;

            // Build GPU buffers
            SetupBuffers(grid, f);
        }

        void Surface::Render()
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            mVao->Bind();
            glDrawElements(mPrimitive, mNumIndices, GL_UNSIGNED_INT, 0);
            mVao->Unbind();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        void Surface::RenderUi()
        {
            ImGui::Text("Name: %s", mName.c_str());
            ImGui::Text("Material: %s", mPasses[RenderPass::Forward].c_str());
            ImGui::Checkbox("Render", &mVisible);
        }

        std::vector<glm::vec3> Surface::Points()
        {
            // Get vbo data
            glm::vec3* data = mVbo->MapBuffer(GL_READ_WRITE);

            // Create vector
            std::vector<glm::vec3> dataVec(data, data + mDataSize);

            // Unmap vbo
            mVbo->UnmapBuffer();

            return dataVec;
        }

        void Surface::SaveObj(const std::string& filename)
        {
            //std::ofstream out(filename);

            //// Write vertex positions
            //for (const auto& v : vertices) {
            //    out << "v " << v.x << " " << v.y << " " << v.z << "\n";
            //}

            //// Write triangle faces (1-based indexing for OBJ)
            //for (const auto& tri : indices) {
            //    out << "f " << tri.x + 1 << " " << tri.y + 1 << " " << tri.z + 1 << "\n";
            //}

            //out.close();
        }

        void Surface::SetupBuffers(const std::shared_ptr<geom::Grid2D>& grid, std::function<float(glm::vec2)> f)
        {
            // Get domain info
            glm::vec2 origin = { grid->BoundingBox().LimitsX[0], grid->BoundingBox().LimitsY[0] };
            glm::vec2 ds = { grid->Ds().x, grid->Ds().y };
            int2 dim = make_int2(grid->Nx(), grid->Ny());

            // Temporal buffers
            std::vector<int> indices(6 * (dim.x - 1) * (dim.y - 1));
            std::vector<glm::vec3> vertices(dim.x * dim.y);

            // Generate vertices (evaluate surface at grid center)
            for (int i = 0; i < dim.x; i++)
            {
                for (int k = 0; k < dim.y; k++)
                {
                    glm::vec2 p = origin + glm::vec2(i + 0.5f, k + 0.5f) * ds;
                    float y = f(p);

                    vertices[XM::Index1D(make_int2(i, k), dim)] = { p.x, y, p.y };
                }
            }

            // Generate triangle indices
            for (int i = 0; i < dim.x - 1; i++)
            {
                for (int k = 0; k < dim.y - 1; k++)
                {
                    const int flatIdx = XM::Index1D(make_int2(i, k), dim - 1);

                    int i0 = XM::Index1D(make_int2(i + 0, k + 0), dim);
                    int i1 = XM::Index1D(make_int2(i + 1, k + 0), dim);
                    int i2 = XM::Index1D(make_int2(i + 0, k + 1), dim);
                    int i3 = XM::Index1D(make_int2(i + 1, k + 1), dim);

                    // Two triangles per quad
                    indices[6 * flatIdx + 0] = i0;
                    indices[6 * flatIdx + 1] = i1;
                    indices[6 * flatIdx + 2] = i2;

                    indices[6 * flatIdx + 3] = i1;
                    indices[6 * flatIdx + 4] = i3;
                    indices[6 * flatIdx + 5] = i2;
                }
            }

            mNumIndices = 6 * (dim.x - 1) * (dim.y - 1);
            mDataSize = dim.x * dim.y;

            // Sends data to opengl
            mEbo = std::make_shared<Vbo<int>>(indices, GL_STATIC_DRAW, GL_ELEMENT_ARRAY_BUFFER);
            mVbo = std::make_shared<Vbo<glm::vec3>>(vertices, GL_DYNAMIC_DRAW);

            mVao = std::make_shared<Vao>(std::vector<VertexAttrib>{VertexAttrib(mVbo, 0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0)});
            mVao->BindEbo(mEbo);
        }

        void Surface::SetupBuffers(const std::shared_ptr<geom::Grid3D>& grid, std::function<float(glm::vec2)> f)
        {
            // Get domain info
            glm::vec2 origin = { grid->BoundingBox().LimitsX[0], grid->BoundingBox().LimitsZ[0] };
            glm::vec2 ds = { grid->Ds().x, grid->Ds().z };
            int2 dim = make_int2(grid->Nx(), grid->Nz());

            // Temporal buffers
            std::vector<int> indices(6 * (dim.x - 1) * (dim.y - 1));
            std::vector<glm::vec3> vertices(dim.x * dim.y);

            // Generate vertices (evaluate surface at grid center)
            for (int i = 0; i < dim.x; i++) 
            {
                for (int k = 0; k < dim.y; k++) 
                {
                    glm::vec2 p = origin + glm::vec2(i + 0.5f, k + 0.5f) * ds;
                    float y = f(p);

                    vertices[XM::Index1D(make_int2(i, k), dim)] = { p.x, y, p.y };
                }
            }

            // Generate triangle indices
            for (int i = 0; i < dim.x - 1; i++)
            {
                for (int k = 0; k < dim.y - 1; k++)
                {
                    const int flatIdx = XM::Index1D(make_int2(i, k), dim - 1);

                    int i0 = XM::Index1D(make_int2(i + 0, k + 0), dim);
                    int i1 = XM::Index1D(make_int2(i + 1, k + 0), dim);
                    int i2 = XM::Index1D(make_int2(i + 0, k + 1), dim);
                    int i3 = XM::Index1D(make_int2(i + 1, k + 1), dim);

                    // Two triangles per quad
                    indices[6 * flatIdx + 0] = i0;
                    indices[6 * flatIdx + 1] = i1;
                    indices[6 * flatIdx + 2] = i2;

                    indices[6 * flatIdx + 3] = i1;
                    indices[6 * flatIdx + 4] = i3;
                    indices[6 * flatIdx + 5] = i2;
                }
            }

            mNumIndices = 6 * (dim.x - 1) * (dim.y - 1);
            mDataSize = dim.x * dim.y;

            // Sends data to opengl
            mEbo = std::make_shared<Vbo<int>>(indices, GL_STATIC_DRAW, GL_ELEMENT_ARRAY_BUFFER);
            mVbo = std::make_shared<Vbo<glm::vec3>>(vertices, GL_DYNAMIC_DRAW);

            mVao = std::make_shared<Vao>(std::vector<VertexAttrib>{VertexAttrib(mVbo, 0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0)});
            mVao->BindEbo(mEbo);
        }
    } // namespace geom
} // namespace X3