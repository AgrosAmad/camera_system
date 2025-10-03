// X3
#include <MeshLoader.h>
#include <Mesh.h>

namespace X3
{
    namespace utils
    {
        std::shared_ptr<geom::Mesh> LoadModel(std::string fName, std::shared_ptr<Scene> xeScene)
        {
            // Assimp loader
            Assimp::Importer import;
            const aiScene * scene = import.ReadFile(fName, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

            // Check for errors
            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
                std::cout << fName << std::endl;
                return nullptr;
            }

            // Directory and object names
            std::string directory = fName.substr(0, fName.find_last_of('/'));
            std::string name = fName.substr(fName.find_last_of('/') + 1, fName.find_last_of('.') - fName.find_last_of('/') - 1);

            // Only one object
            if (scene->mNumMeshes == 1)
            {
                // Generates XE mesh
                aiMesh* mesh = scene->mMeshes[0];
                std::shared_ptr<geom::Mesh> xeMesh = ProcessMesh(directory, mesh, scene, name);

                // Send to XE scene
                xeScene->Add(xeMesh);

                return xeMesh;
            }
            // Multiple objects, load iteratively
            else
            {
                ProcessNode(directory, scene->mRootNode, scene, xeScene, nullptr);
            }

            return nullptr;

        }

        void LoadMatMap(const std::string& dir, aiMaterial* mat, std::map<MapType, std::weak_ptr<Texture2D>>& map, aiTextureType aiType, MapType type)
        {
            // Unwrap
            Textures2D& textures = Textures2D::Inst();

            // Loads just first texture TODO: Handle multiple textures
            if (mat != nullptr && mat->GetTextureCount(aiType) > 0)
            {

                // Gets file name
                aiString str;
                mat->GetTexture(aiType, 0, &str);

                // Texture name is simply the file's path
                std::string tName = std::string(str.C_Str());

                // Loads material
                map[type] = textures.Load(dir + "/" + tName, tName);
            }
        }

        std::map<MapType, std::weak_ptr<Texture2D>> LoadMatMaps(const std::string& dir, aiMaterial* mat)
        {
            // Prepares data
            std::map<MapType, std::weak_ptr<Texture2D>> matMaps;
            LoadMatMap(dir, mat, matMaps, aiTextureType_DIFFUSE, MapType::Diffuse);
            LoadMatMap(dir, mat, matMaps, aiTextureType_SPECULAR, MapType::Specular);
            LoadMatMap(dir, mat, matMaps, aiTextureType_EMISSIVE, MapType::Emission);

            return matMaps;
        }

        void ProcessNode(const std::string& dir, aiNode* node, const aiScene* scene, std::shared_ptr<Scene> xeScene, std::shared_ptr<Renderable> meshNode)
        {

            // Process meshes in this node
            if (node->mNumMeshes > 0)
            {
                // Loops over all meshes        
                for (int meshIdx = 0; meshIdx < node->mNumMeshes; meshIdx++)
                {
                    // Generates X3 mesh
                    aiMesh* mesh = scene->mMeshes[node->mMeshes[meshIdx]];
                    std::shared_ptr<X3::geom::Mesh> xeMesh = ProcessMesh(dir, mesh, scene);

                    // Send to X3 scene
                    xeScene->Add(xeMesh);

                    // Set parent-child relations
                    if (meshNode)
                    {
                        xeMesh->Parent(meshNode);
                        meshNode->Children(xeMesh);
                    }
                }
            }

            // Process children nodes
            for (int i = 0; i < node->mNumChildren; i++)
            {
                ProcessNode(dir, node->mChildren[i], scene, xeScene, nullptr);
            }
        }

        std::shared_ptr<X3::geom::Mesh> ProcessMesh(const std::string& dir, aiMesh* mesh, const aiScene* scene, const std::string& name)
        {
            // Managers
            Materials& materials = Materials::Inst();
            Names& names = Names::Inst();

            // Prepares containers
            std::vector<geom::Vertex> vertices;
            std::vector<int> indices;

            // Process vertices
            for (int i = 0; i < mesh->mNumVertices; i++)
            {
                geom::Vertex vertex;

                vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
                vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

                // Checks if we have texture coordinates
                if (mesh->mTextureCoords[0])
                {
                    vertex.TexCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
                }
                else
                {
                    vertex.TexCoord = glm::vec2(0.f);
                }

                vertices.push_back(vertex);
            }

            // Process indices
            for (int i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
                for (int j = 0; j < face.mNumIndices; j++)
                {
                    indices.push_back(face.mIndices[j]);
                }
            }

            // Process material maps
            aiMaterial* aiMat = mesh->mMaterialIndex >= 0 ? scene->mMaterials[mesh->mMaterialIndex] : nullptr;
            std::map<X3::MapType, std::weak_ptr<X3::Texture2D>> matMaps = LoadMatMaps(dir, aiMat);

            // Mesh and material names
            std::string meshName = name.empty() ? names.Insert(mesh->mName.C_Str()) : names.Insert(name);
            std::string matName = aiMat == nullptr ? meshName : aiMat->GetName().C_Str();
            if (matName == "DefaultMaterial") matName = meshName; // replaces empty assimp material name

            // Generates XE mesh and its material
            std::shared_ptr<geom::Mesh> xMesh = std::make_shared<geom::Mesh>(vertices, indices, meshName);
            std::shared_ptr<MeshMaterial> material = std::dynamic_pointer_cast<MeshMaterial>(materials.Load(matName, MatType::MeshMat));
            //std::shared_ptr<SolidMaterial> material = std::dynamic_pointer_cast<SolidMaterial>(materials.Load(matName, MatType::SolidMat));

            material->MatMaps(matMaps);

            // Cross relation between material and mesh
            xMesh->SetPass(RenderPass::Forward, matName);
            material->User(xMesh);

            return xMesh;
        }
    } // ends namespace utils
} // ends namespace X3