// X3
#include <Parameters.h>
#include <Shaders.h>
#include <Scene.h>
#include <Light.h>
#include <Names.h>
#include <Gui.h>

// X3: Geometry
#include <Trapezoid.h>
#include <Surface.h>
#include <Curve2D.h>
#include <Grid1D.h>
#include <Grid2D.h>
#include <Grid3D.h>
#include <Points2D.h>
#include <Points.h>
#include <Cube.h>
#include <Mesh.h>
#include <Box.h>

namespace X3
{
    void Scene::Clear()
    {
        mRenderables.clear();
        mCameras.clear();
        mLights.clear();
    }

    void Scene::Update()
    {
        // Add objects
        for (auto& e : mAdd)
        {
            if (std::dynamic_pointer_cast<Camera>(e) != nullptr)
            {
                mCameras.push_back(std::dynamic_pointer_cast<Camera>(e));
            }
            else if (std::dynamic_pointer_cast<Light>(e) != nullptr)
            {
                mLights.push_back(std::dynamic_pointer_cast<Light>(e));
            }
            else if (std::dynamic_pointer_cast<Renderable>(e) != nullptr)
            {
                mRenderables.push_back(std::dynamic_pointer_cast<Renderable>(e));
            }
        }

        mAdd.clear();

        // Remove objects
        for (auto& e : mRemove)
        {
            if (std::dynamic_pointer_cast<Camera>(e) != nullptr)
            {
                utils::EraseFromVector(e, mCameras);
            }
            else if (std::dynamic_pointer_cast<Light>(e) != nullptr)
            {
                utils::EraseFromVector(e, mLights);
            }
            else if (std::dynamic_pointer_cast<Renderable>(e) != nullptr)
            {
                utils::EraseFromVector(e, mRenderables);
            }
        }

        mRemove.clear();

        // Update all objects
        for (auto& c : mCameras) c->Update();
        for (auto& l : mLights) l->Update();
        for (auto& r : mRenderables) r->Update();

    }

    void Scene::Load()
    {
        if (mLoader) mLoader(shared_from_this());
    }

    void Scene::SaveBinary(const std::string& fn) const
    {

        std::ofstream out(fn, std::ios::binary);
        out.close();
    }

    void Scene::LoadBinary(const std::string& fn)
    {
    }

    std::shared_ptr<geom::Points2D> Scene::AddPoints2D(const std::vector<glm::vec2> &positions, const glm::vec3 &color)
    {
        // Managers
        Names& names = Names::Inst();

        // Create geometry
        std::string name = names.Insert("points");
        std::shared_ptr<geom::Points2D> points = std::make_shared<geom::Points2D>(positions, color, name);

        // Handles material and add to scene
        AddGeneral(points, MatType::Point2DMat);

        return points;
    }

    std::shared_ptr<geom::Points> Scene::AddPoints(const std::vector<glm::vec3> &positions, const glm::vec3 &color)
    {
        // Managers
        Names& names = Names::Inst();

        // Create geometry
        std::string name = names.Insert("points");
        std::shared_ptr<geom::Points> points = std::make_shared<geom::Points>(positions, color, name);

        // Handles material and add to scene
        AddGeneral(points, MatType::PointMat);

        return points;

    }

    std::shared_ptr<geom::Trapezoid> Scene::AddTrapezoid(const std::vector<glm::vec3>& vertices)
    {
        // Managers
        Names& names = Names::Inst();

        // Create geometry
        std::string name = names.Insert("trapezoid");
        std::shared_ptr<geom::Trapezoid> trapezoid = std::make_shared<geom::Trapezoid>(name, vertices);

        // Handles material and add to scene
        AddGeneral(trapezoid, MatType::BoxMat);

        return trapezoid;
    }

    std::shared_ptr<geom::Mesh> Scene::AddMesh(const std::string& fName)
    {
        return utils::LoadModel(fName, shared_from_this());
    }

    std::shared_ptr<geom::Grid3D> Scene::AddGrid3D(const float& xMin, const float& xMax, const float& yMin, const float& yMax, const float& zMin, const float& zMax, const int& nx, const int& ny, const int& nz)
    {
        // // Managers
        // Names& names = Names::Inst();

        // // Create geometry
        // std::string name = names.Insert("grid3D");
        // std::shared_ptr<geom::Grid3D> grid3D = std::make_shared<geom::Grid3D>(name, XM::AABB(xMin, xMax, yMin, yMax, zMin, zMax), nx, ny, nz);

        // // Handles material and add to scene
        // AddGeneral(grid3D, MatType::Grid3DMat);

        // return grid3D;
    }

    std::shared_ptr<geom::Grid3D> Scene::AddGrid3D(const float& min, const float& max, const int& n)
    {
        return AddGrid3D(min, max, min, max, min, max, n, n, n);
    }

    std::shared_ptr<geom::Grid2D> Scene::AddGrid2D(const float& xMin, const float& xMax, const float& yMin, const float& yMax, const int& nx, const int& ny)
    {
        // Managers
        Names& names = Names::Inst();

        // Create geometry
        std::string name = names.Insert("grid2D");
        std::shared_ptr<geom::Grid2D> grid2D = std::make_shared<geom::Grid2D>(name, XM::AABB(xMin, xMax, yMin, yMax, 0.f, 0.f), nx, ny);

        // Handles material and add to scene
        AddGeneral(grid2D, MatType::Grid2DMat);

        return grid2D;
    }

    std::shared_ptr<geom::Grid2D> Scene::AddGrid2D(const float& min, const float& max, const int& n)
    {
        return AddGrid2D(min, max, min, max, n, n);
    }

    std::shared_ptr<geom::Grid1D> Scene::AddGrid1D(const float &xMin, const float &xMax, const int &nx)
    {
        // Managers
        Names& names = Names::Inst();

        // Create geometry
        std::string name = names.Insert("grid1D");
        std::shared_ptr<geom::Grid1D> grid = std::make_shared<geom::Grid1D>(name, xMin, xMax, nx);

        // Handles material and add to scene
        AddGeneral(grid, MatType::Grid1DMat);

        return grid;
    }

    std::shared_ptr<geom::Surface> Scene::AddSurface(const std::shared_ptr<geom::Grid2D>& grid, std::function<float(glm::vec2)> f)
    {
        // // Managers
        // Names& names = Names::Inst();

        // // Create geometry
        // std::string name = names.Insert("surface");
        // std::shared_ptr<geom::Surface> surface = std::make_shared<geom::Surface>(name, grid, f);

        // // Handles material and add to scene
        // AddGeneral(surface, MatType::SurfaceMat);

        // return surface;
    }

    std::shared_ptr<geom::Surface> Scene::AddSurface(const std::shared_ptr<geom::Grid3D>& grid, std::function<float(glm::vec2)> f)
    {
        // // Managers
        // Names& names = Names::Inst();

        // // Create geometry
        // std::string name = names.Insert("surface");
        // std::shared_ptr<geom::Surface> surface = std::make_shared<geom::Surface>(name, grid, f);

        // // Handles material and add to scene
        // AddGeneral(surface, MatType::SurfaceMat);

        // return surface;
    }

    std::shared_ptr<geom::Curve2D> Scene::AddCurve2D(const std::shared_ptr<geom::Grid1D> &grid, std::function<float(float)> f)
    {
        // Managers
        Names& names = Names::Inst();

        // Create geometry
        std::string name = names.Insert("curve2D");
        std::shared_ptr<geom::Curve2D> line = std::make_shared<geom::Curve2D>(name, grid, f);

        // Handles material and add to scene
        AddGeneral(line, MatType::Curve2DMat);

        return line;
    }

    std::shared_ptr<geom::Curve2D> Scene::AddCurve2D(const std::shared_ptr<geom::Grid2D> &grid, std::function<float(float)> f)
    {
        // Managers
        Names& names = Names::Inst();

        // Create geometry
        std::string name = names.Insert("curve2D");
        std::shared_ptr<geom::Curve2D> line = std::make_shared<geom::Curve2D>(name, grid, f);

        // Handles material and add to scene
        AddGeneral(line, MatType::Curve2DMat);

        return line;
    }

    std::shared_ptr<geom::Cube> Scene::AddCube()
    {
        // Managers
        Names& names = Names::Inst();

        // Create geometry
        std::string name = names.Insert("cube");
        std::shared_ptr<geom::Cube> cube = std::make_shared<geom::Cube>(name);

        // Handles material and add to scene
        AddGeneral(cube, MatType::MeshMat);

        return cube;
    }

    std::shared_ptr<geom::Box> Scene::AddBox()
    {
        // Managers
        Names& names = Names::Inst();

        // Create geometry
        std::string name = names.Insert("box");
        std::shared_ptr<geom::Box> box = std::make_shared<geom::Box>(name);

        // Handles material and add to scene
        AddGeneral(box, MatType::BoxMat);

        return box;
    }

    void Scene::AddGeneral(std::shared_ptr<Renderable> e, MatType matType)
    {
        if (matType != MatType::EmptyMat)
        {
            // Managers
            Materials& materials = Materials::Inst();

            // Ensures unique material name
            std::string name = e->Name();

            // Assign chosen material type
            std::shared_ptr<Material> material = materials.Load(name, matType);

            // Cross relation between material-mesh
            e->SetPass(RenderPass::Forward, name);
            material->User(e);
        }

        // Add to scene
        Add(e);
    }

    void Scene::AddLight(const LightType& type)
    {
        Names& names = Names::Inst();

        Add(std::make_shared<Light>(names.Insert("light"), type));
    }

    void Scene::Reload()
    {
        Clear();
        Load();
    }

} // ends namespace X3