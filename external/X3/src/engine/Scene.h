#pragma once

// STL
#include <functional>
#include <fstream>
#include <vector>
#include <memory>

// X3
#include <Renderable.h>
#include <STLUtils.h>
#include <Camera.h>
#include <Light.h>

// JSON
#include <nlohmann/json.hpp>

namespace X3
{
	// Cross definition
	namespace geom
	{
		class Trapezoid;
		class Grid1D;
		class Grid2D;
		class Grid3D;
		class Surface;
		class Curve2D;
		class Points;
		class Points2D;
		class Cube;
		class Mesh;
		class Box;
	}

	// We use "enable_shared_from_this" to enable callback loaders
	class Scene : public std::enable_shared_from_this<Scene>
	{
	public:

		// Common methods
		void Update();
		void Reload();
		void Clear();
		void Load();

		// Save/Load
		void CallbackLoader(std::function<void(std::shared_ptr<Scene>)> c) { mLoader = c; }
		void SaveBinary(const std::string& fn) const;
		void LoadBinary(const std::string& fn);


		// Add/Remove scene objects

		// General entities
		void Remove(std::shared_ptr<Renderable> e) { mRemove.push_back(e); }
		void Add(std::shared_ptr<Renderable> e) { mAdd.push_back(e); }

		// Simulation geometries
		std::shared_ptr<geom::Grid3D> AddGrid3D(const float& xMin = -1.f, const float& xMax = 1.f, const float& yMin = -1.f, const float& yMax = 1.f, const float& zMin = -1.f, const float& zMax = 1.f, const int& nx = 32, const int& ny = 32, const int& nz = 32);
		std::shared_ptr<geom::Grid3D> AddGrid3D(const float& min = -1.f, const float& max = 1.f, const int& n = 32);

		std::shared_ptr<geom::Grid2D> AddGrid2D(const float &xMin = -1.f, const float &xMax = 1.f, const float &yMin = -1.f, const float &yMax = 1.f, const int &nx = 32, const int &ny = 32);
		std::shared_ptr<geom::Grid2D> AddGrid2D(const float& min = -1.f, const float& max = 1.f, const int& n = 32);

		std::shared_ptr<geom::Grid1D> AddGrid1D(const float &xMin = -1.f, const float &xMax = 1.f, const int &nx = 32);

		std::shared_ptr<geom::Surface> AddSurface(const std::shared_ptr<geom::Grid2D> &grid, std::function<float(glm::vec2)> f);
		std::shared_ptr<geom::Surface> AddSurface(const std::shared_ptr<geom::Grid3D>& grid, std::function<float(glm::vec2)> f);
		std::shared_ptr<geom::Curve2D> AddCurve2D(const std::shared_ptr<geom::Grid1D> &grid, std::function<float(float)> f);
		std::shared_ptr<geom::Curve2D> AddCurve2D(const std::shared_ptr<geom::Grid2D> &grid, std::function<float(float)> f);

		// Points/Particles
		std::shared_ptr<geom::Points2D> AddPoints2D(const std::vector<glm::vec2>& positions, const glm::vec3& color = glm::vec3(0.f, 0.f, 1.f));
		std::shared_ptr<geom::Points> AddPoints(const std::vector<glm::vec3>& positions, const glm::vec3& color = glm::vec3(0.f, 0.f, 1.f));

		// Standard geometries
		std::shared_ptr<geom::Trapezoid> AddTrapezoid(const std::vector<glm::vec3>& vertices = {});
		std::shared_ptr<geom::Mesh> AddMesh(const std::string& fName);
		std::shared_ptr<geom::Cube> AddCube();
		std::shared_ptr<geom::Box> AddBox();

		void AddGeneral(std::shared_ptr<Renderable> e, MatType matType);

		// Lights
		void AddLight(const LightType& type);

		// Cameras

		// Get/Set
		std::vector<std::shared_ptr<Renderable>>& Renderables() { return mRenderables; }
		std::vector<std::shared_ptr<Camera>>& Cameras() { return mCameras; }
		std::vector<std::shared_ptr<Light>>& Lights() { return mLights; }

	private:

		// Scene data
		std::vector<std::shared_ptr<Renderable>> mRenderables;
		std::vector<std::shared_ptr<Camera>> mCameras;
		std::vector<std::shared_ptr<Light>> mLights;

		// Update scene
		std::vector<std::shared_ptr<Renderable>> mRemove;
		std::vector<std::shared_ptr<Renderable>> mAdd;

		// Loader
		std::function<void(std::shared_ptr<Scene>)> mLoader = nullptr;

		// Helpers

	};
} // namespace X3