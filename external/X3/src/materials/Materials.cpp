#include <Materials.h>

namespace X3
{
    Materials& Materials::Inst()
    {
        static Materials mats;
        return mats;
    }


    std::shared_ptr<Material> Materials::Load(const std::string& key, MatType type)
    {

        // Material already loaded
        if (ContainsMaterial(key))
        {
            return mMatCache[key];
        }

        // Generate new material
        Shaders& shaders = Shaders::Inst();
        std::string rootDir = Parameters::Inst().Main.RootDir;

        std::shared_ptr<Shader> shader = nullptr;

        // Loads corresponding material and shader pair
        switch (type)
        {
        case MatType::MeshMat:
        {
            mMatCache[key] = std::make_shared<MeshMaterial>(key);
            shader = shaders.Load("MeshFw", rootDir + "/shaders/objects/MeshFw");
            break;
        }
        case MatType::DepthMat:
        {
            mMatCache[key] = std::make_shared<DepthMaterial>(key);
            shader = shaders.Load("DepthBuffer", rootDir + "/shaders/objects/DepthBuffer");
            break;
        }
        case MatType::SolidMat:
        {
            mMatCache[key] = std::make_shared<SolidMaterial>(key);
            shader = shaders.Load("SolidColor", rootDir + "/shaders/objects/SolidColor");
            break;
        }
        case MatType::BoxMat:
        {
            mMatCache[key] = std::make_shared<BoxMaterial>(key);
            shader = shaders.Load("Box", rootDir + "/shaders/objects/Box");
            break;
        }
        case MatType::PointMat:
        {
            mMatCache[key] = std::make_shared<PointMaterial>(key);
            shader = shaders.Load("Points", { rootDir + "/shaders/objects/Points.vert",  rootDir + "/shaders/objects/Points.geom", rootDir + "/shaders/objects/Points.frag" });
            break;
        }
        case MatType::Point2DMat:
        {
            mMatCache[key] = std::make_shared<PointMaterial>(key);
            shader = shaders.Load("Points2D", { rootDir + "/shaders/objects/Points2D.vert",  rootDir + "/shaders/objects/Points2D.geom", rootDir + "/shaders/objects/Points2D.frag" });
            break;
        }
        case MatType::Grid1DMat:
        {
            mMatCache[key] = std::make_shared<GridMaterial>(key);
            shader = shaders.Load("Grid1D", rootDir + "/shaders/objects/Grid1D");
            break;
        }
        case MatType::Grid2DMat:
        {
            mMatCache[key] = std::make_shared<GridMaterial>(key);
            shader = shaders.Load("Grid2D", rootDir + "/shaders/objects/Grid2D");
            break;
        }
        // case MatType::Grid3DMat:
        // {
        //     mMatCache[key] = std::make_shared<GridMaterial>(key);
        //     shader = shaders.Load("Grid3D", rootDir + "/shaders/objects/Grid3D");
        //     break;
        // }
        // case MatType::SurfaceMat:
        // {
        //     mMatCache[key] = std::make_shared<SurfaceMaterial>(key);
        //     shader = shaders.Load("Surface", rootDir + "/shaders/objects/Surface");
        //     break;
        // }
        case MatType::Curve2DMat:
        {
            mMatCache[key] = std::make_shared<SurfaceMaterial>(key);
            shader = shaders.Load("Curve2D", rootDir + "/shaders/objects/Curve2D");
            break;
        }
        case MatType::Shallow2DMat:
        {
            mMatCache[key] = std::make_shared<Shallow2DMaterial>(key);
            shader = shaders.Load("Shallow2D", rootDir + "/shaders/objects/Shallow2D");
            break;
        }
        case MatType::Potential2DMat:
        {
            mMatCache[key] = std::make_shared<Potential2DMaterial>(key);
            shader = shaders.Load("Potential2D", { rootDir + "/shaders/objects/Potential2D.vert", rootDir + "/shaders/objects/Potential2D.frag" });
            break;
        }
        default:
            break;
        }

        // Shader-material relation
        shader->AddMaterial(mMatCache[key]);

        return mMatCache[key];

    }

    std::shared_ptr<Material> Materials::GetMaterial(const std::string& key) const
    {
        if (!ContainsMaterial(key))
        {
            std::cout << "Attempting to get non-existing material with key '" + key + "'!\n";
            return nullptr;
        }

        return mMatCache.at(key);
    }

    void Materials::Update()
    {
        // Update all materials in cache
        for (auto& mat : mMatCache) mat.second->Update();

        // Deletes materials with no object
        for (auto it = mMatCache.begin(); it != mMatCache.end(); )
        {
            if (it->second->NumUsers() == 0)
            {
                it = mMatCache.erase(it); // Erase element and update the iterator
            }
            else
            {
                ++it; // Move to the next element
            }
        }

    }

    void Materials::ClearCache()
    {
        mMatCache.clear();
    }

    bool Materials::ContainsMaterial(const std::string& key) const
    {
        return mMatCache.count(key) > 0;
    }
} // namespace X3