// STL
#include <iostream>
#include <stdexcept>

// X3
#include <Shaders.h>

namespace X3
{
    Shaders& Shaders::Inst()
    {
        static Shaders spm;
        return spm;
    }

    void Shaders::Update()
    {
        for (auto& shader : mFwdCache) shader.second->Update();
    }

    std::shared_ptr<Shader> Shaders::Load(const std::string& key, const std::vector<std::string>& shadersFn)
    {
        if (!ContainsShaderProgram(key)) mFwdCache[key] = std::make_shared<Shader>(shadersFn);

        return mFwdCache.at(key);
    }

    std::shared_ptr<Shader> Shaders::Load(const std::string& key, const std::string mainPath)
    {
        std::vector<std::string> shadersFn = { mainPath + ".vert", mainPath + ".frag" };
        return Load(key, shadersFn);
    }

    std::shared_ptr<Shader> Shaders::GetProgram(const std::string& key) const
    {
        if (!ContainsShaderProgram(key))
        {
            std::cout << "Attempting to get non-existing shader program with key '" + key + "'!\n";
            return nullptr;
        }

        return mFwdCache.at(key);
    }

    void Shaders::ClearProgramCache()
    {
        mFwdCache.clear();
    }

    bool Shaders::ContainsShaderProgram(const std::string& key) const
    {
        return mFwdCache.count(key) > 0;
    }
}
