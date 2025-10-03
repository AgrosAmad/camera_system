// X3
#include <Renderable.h>
#include <STLUtils.h>
#include <Material.h>
#include <Camera.h>
#include <Light.h>

namespace X3
{
	void Material::Update()
	{
		// Erase expired users
		utils::EraseExpiredPointers(mUsers);
	}

	void Material::RenderUsers(std::shared_ptr<Shader>& shader)
	{
		// Render all objects associated to this material
		for (int i = 0; i < mUsers.size(); i++)
		{
			auto lockedUser = mUsers[i].lock();
			if(lockedUser->Visible())
			{
				shader->Unif("model") = lockedUser->MatM();
				lockedUser->Render();
			}
		}
	}

	MeshMaterial::MeshMaterial(const std::string& name)
	{
		// Set values
		mColor = glm::vec4(1.f);
		mShininess = 32.f;
		mMatMaps = {};
		mName = name;

		// Checks available materials
		mHasMaterial[0] = false;
		mHasMaterial[1] = false;
		mHasMaterial[2] = false;
	}

	void MeshMaterial::Render(std::shared_ptr<Shader>&& shader)
	{

		// Setup
		Textures2D& textures = Textures2D::Inst();
		Parameters& params = Parameters::Inst();

		// Binds available materials/textures
		shader->Unif("material.HasMaterial[0]") = mHasMaterial[0];
		shader->Unif("material.HasMaterial[1]") = mHasMaterial[1];
		shader->Unif("material.HasMaterial[2]") = mHasMaterial[2];

		int texCounter = 0;
		for (const auto& pair : mMatMaps) pair.second.lock()->Bind(GL_TEXTURE0 + texCounter++);

		// Set other material properties
		shader->Unif("material.diffuse") = 0;
		shader->Unif("material.specular") = 1;
		shader->Unif("material.emission") = 2;
		shader->Unif("material.shininess") = mShininess;
		shader->Unif("material.Color") = mColor;

		// Render all objects associated to this material
		RenderUsers(shader);

	}

	void MeshMaterial::MatMaps(std::map<MapType, std::weak_ptr<Texture2D>>& matMaps)
	{
		mMatMaps = matMaps;

		mHasMaterial[0] = mMatMaps.count(MapType::Diffuse) > 0;
		mHasMaterial[1] = mMatMaps.count(MapType::Specular) > 0;
		mHasMaterial[2] = mMatMaps.count(MapType::Emission) > 0;
	}

	DepthMaterial::DepthMaterial(const std::string& name)
	{
		mName = name;
	}

	void DepthMaterial::Render(std::shared_ptr<Shader>&& shader)
	{

		// Setup
		Parameters& params = Parameters::Inst();

		// Render all objects associated to this material
		RenderUsers(shader);
	}

	SolidMaterial::SolidMaterial(const std::string& name)
	{

		// Default params
		mColor = glm::vec3(0.f, 0.f, 1.f);
		mName = name;
	}

	void SolidMaterial::Render(std::shared_ptr<Shader>&& shader)
	{
		// Setup
		Parameters& params = Parameters::Inst();

		// Set solid color
		shader->Unif("Color") = mColor;

		// Render all objects associated to this material
		RenderUsers(shader);
	}

	PointMaterial::PointMaterial(const std::string& name)
	{
		mName = name;
		mRadius = 0.1f;
	}

	void PointMaterial::Render(std::shared_ptr<Shader>&& shader)
	{
		// Set particle radius
		shader->Unif("SphereRadius") = mRadius;

		// Render all objects associated to this material
		RenderUsers(shader);
	}

	BoxMaterial::BoxMaterial(const std::string& name)
	{
		mColor = glm::vec3(0.f, 0.f, 1.f);
		mName = name;

	}

	void BoxMaterial::Render(std::shared_ptr<Shader>&& shader)
	{
		// Set box color
		shader->Unif("BoxColor") = mColor;

		// Render all objects associated to this material
		RenderUsers(shader);
	}

    GridMaterial::GridMaterial(const std::string &name)
    {
		mName = name;
		mColor = glm::vec3(0.f, 0.f, 1.f);
    }

    void GridMaterial::Render(std::shared_ptr<Shader> &&shader)
    {
		// Set box color
		shader->Unif("MeshColor") = mColor;

		// Render all objects associated to this material
		RenderUsers(shader);
    }

    SurfaceMaterial::SurfaceMaterial(const std::string &name)
    {
		mName = name;
		mColor = glm::vec3(0.f, 1.f, 1.f);
	}

	void SurfaceMaterial::Render(std::shared_ptr<Shader> &&shader)
    {
		// Set line color
		shader->Unif("SurfColor") = mColor;
		RenderUsers(shader);
	}

    Shallow2DMaterial::Shallow2DMaterial(const std::string &name)
    {

		mName = name;
		mColor = glm::vec3(0.f, 1.f, 1.f);
	}

	void Shallow2DMaterial::Render(std::shared_ptr<Shader> &&shader)
    {
		// Set line color and texture
		shader->Unif("LineColor") = mColor;
		RenderUsers(shader);
    }

    Potential2DMaterial::Potential2DMaterial(const std::string &name)
    {
		Shaders& shaders = Shaders::Inst();
        std::string rootDir = Parameters::Inst().Main.RootDir;

		mColorMap = Textures2D::Inst().Load(rootDir + "/textures/colormap.png", "colormap", 9987U, 9729U, GL_CLAMP_TO_EDGE);
		mName = name;
		this->Set("MaxPot", 1.f);
		this->Set("MinPot", 0.f);
    }

    void Potential2DMaterial::Render(std::shared_ptr<Shader> &&shader)
    {
		// Set line color and texture
		mColorMap.lock()->Bind(GL_TEXTURE0);

		shader->Unif("MaxPot") = this->Get<float>("MaxPot");
		shader->Unif("MinPot") = this->Get<float>("MinPot");
		shader->Unif("ColorMap") = 0;

		RenderUsers(shader);
    }

    // class Material
} // namespace X3