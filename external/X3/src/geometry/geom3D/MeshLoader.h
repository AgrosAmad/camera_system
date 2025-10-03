#pragma once

// Assimp
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

// XE
#include <Scene.h>
#include <Names.h>

// Cross definition
namespace X3
{
	namespace geom
	{
		class Mesh;
	}
}

namespace X3
{
	namespace utils
	{
		// Main loader function
		std::shared_ptr<geom::Mesh> LoadModel(std::string fName, std::shared_ptr<Scene> xeScene);

		// Assimp Helpers
		void LoadMatMap(const std::string& dir, aiMaterial* mat, std::map<MapType, std::weak_ptr<Texture2D>>& map, aiTextureType aiType, MapType type);
		std::map<MapType, std::weak_ptr<Texture2D>> LoadMatMaps(const std::string& dir, aiMaterial* mat);
		void ProcessNode(const std::string& dir, aiNode* node, const aiScene* scene, std::shared_ptr<Scene> xeScene, std::shared_ptr<Renderable> meshNode);
		std::shared_ptr<X3::geom::Mesh> ProcessMesh(const std::string& dir, aiMesh* mesh, const aiScene* scene, const std::string& name = "");

	} // namespace utils
} // namespace X3