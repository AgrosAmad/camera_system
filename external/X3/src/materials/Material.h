#pragma once

// STL
#include <iostream>
#include <any>
#include <variant>
#include <map>

// X3
#include <Textures.h>
#include <Shaders.h>


namespace X3
{
	// Cross name
	class Renderable;
	class Camera;
	class Light;

	// Helpers
	enum class MatType { MeshMat, DepthMat, SolidMat, PointMat, Point2DMat, BoxMat, Grid2DMat, Grid1DMat, Curve2DMat, Shallow2DMat, Potential2DMat, EmptyMat};
	enum class MapType { Diffuse, Specular, Emission };

	// General material
	class Material
	{
	public:

		// Common methods
		Material() {}
		void Update();

		virtual void Render(std::shared_ptr<Shader>&& shader) {}

		// Get/Set
		void User(std::shared_ptr<Renderable> user) { mUsers.push_back(user); }
		int NumUsers() { return mUsers.size(); }

		// Set a variable of any type
		template<typename T>
		void Set(const std::string& name, T value) 
		{
			mVariables[name] = value;
		}

		// Get a variable with type checking
		template<typename T>
		T Get(const std::string& name) const 
		{
			auto it = mVariables.find(name);
			if (it != mVariables.end())
			{
				return std::any_cast<T>(it->second);
			}
			throw std::runtime_error("Variable not found or wrong type");
		}

	protected:

		// Material data
		std::vector<std::weak_ptr<Renderable>> mUsers;
		std::map<std::string, std::any> mVariables;
		std::string mName;

		// Helpers
		void RenderUsers(std::shared_ptr<Shader>& shader);

	};

	// Material for normal mesh rendering
	class MeshMaterial : public Material
	{

	public:

		// Common methods
		MeshMaterial(const std::string& name);
		void Render(std::shared_ptr<Shader>&& shader) override;

		// Get/Set
		void MatMaps(std::map<MapType, std::weak_ptr<Texture2D>>& matMaps);

	private:

		// Material data
		std::map<MapType, std::weak_ptr<Texture2D>> mMatMaps;
		bool mHasMaterial[3];
		float mShininess;
		glm::vec4 mColor;

	};

	// Material for z-buffer rendering
	class DepthMaterial : public Material
	{
	public:

		// Common methods
		DepthMaterial(const std::string& name);
		void Render(std::shared_ptr<Shader>&& shader) override;
	};

	// Material for solid color
	class SolidMaterial : public Material
	{
	public:

		// Common methods
		SolidMaterial(const std::string& name);
		void Render(std::shared_ptr<Shader>&& shader) override;

	private:
		glm::vec3 mColor;
	};

	// Material for point rendering
	class PointMaterial : public Material
	{

	public:
		// Common methods
		PointMaterial(const std::string& name);
		void Render(std::shared_ptr<Shader>&& shader) override;

	private:
		float mRadius;
	};

	// Material for rendering empty boxes
	class BoxMaterial : public Material
	{
	public:
		// Common methods
		BoxMaterial(const std::string& name);
		void Render(std::shared_ptr<Shader>&& shader) override;
	private:
		glm::vec3 mColor;
	};

	class GridMaterial : public Material
	{
	public:
		// Common methods
		GridMaterial(const std::string& name);
		void Render(std::shared_ptr<Shader>&& shader) override;
	private:
		glm::vec3 mColor;
	};

	class Shallow2DMaterial : public Material
	{
	public:
		// Common methods
		Shallow2DMaterial(const std::string &name);
		void Render(std::shared_ptr<Shader>&& shader) override;
	private:
		glm::vec3 mColor;
	};

	class Potential2DMaterial : public Material
	{
	public:
		// Common methods
		Potential2DMaterial(const std::string &name);
		void Render(std::shared_ptr<Shader>&& shader) override;
	private:
		std::weak_ptr<Texture2D> mColorMap;
	};

	class SurfaceMaterial : public Material
	{
	public:
		// Common methods
		SurfaceMaterial(const std::string& name);
		void Render(std::shared_ptr<Shader>&& shader) override;
	private:
		glm::vec3 mColor;
	};

} // namespace X3