#include <Textures.h>

namespace X3
{
    Textures2D& Textures2D::Inst()
    {
        static Textures2D tex;
        return tex;
    }

    std::shared_ptr<Texture2D> Textures2D::Load(const std::string& fName, const std::string& key, GLenum filterMin, GLenum filterMag, GLint wrap, int comp)
    {

        std::string nKey;

        // Handles default key using file's name
        if (key.empty())
        {
            size_t lastPoint = fName.find_last_of('.');
            size_t lastSlash = fName.find_last_of('/');
            nKey = fName.substr(lastSlash + 1, lastPoint - lastSlash - 1);
        }
        else
        {
            nKey = key;
        }

        // Checks if already loaded
        if (!ContainsTexture(nKey)) TextureCache[nKey] = std::make_shared<Texture2D>(fName, filterMin, filterMag, wrap, comp);

        return TextureCache.at(nKey);
    }

    std::string Textures2D::Create(const std::string& key, const glm::vec4& color, GLenum filterMin, GLenum filterMag, GLint wrap, int comp)
    {
        // Checks if already loaded
        if (!ContainsTexture(key)) TextureCache[key] = std::make_shared<Texture2D>(color, filterMin, filterMag, wrap, comp);

        return key;
    }

    std::string Textures2D::Default()
    {
        return Create("default");
    }

    std::shared_ptr<Texture2D> Textures2D::GetTexture(const std::string& key) const
    {
        if (!ContainsTexture(key))
        {
            std::cout << "Attempting to get non-existing texture with key '" + key + "'!\n";
            return nullptr;
        }

        return TextureCache.at(key);
    }

    void Textures2D::ClearTextureCache()
    {
        TextureCache.clear();
    }

    bool Textures2D::ContainsTexture(const std::string& key) const
    {
        return TextureCache.count(key) > 0;
    }
}