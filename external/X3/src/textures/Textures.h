#pragma once

// STL
#include <iostream>
#include <string>
#include <map>
#include <memory>

// XE
#include <Texture2D.h>

namespace X3
{

	// Singleton class that manages and keeps track of all textures.
	class Textures2D
	{

	public:

		// Gets the one and only instance of the texture manager.
		static Textures2D& Inst();

		// Loads new texture and stores it with specified key.
		std::shared_ptr<Texture2D> Load(const std::string& fName, const std::string& key = "", GLenum filterMin = GL_LINEAR_MIPMAP_LINEAR, GLenum filterMag = GL_LINEAR, GLint wrap = GL_REPEAT, int comp = STBI_rgb_alpha);

		// Creates texture of a given color
		std::string Create(const std::string& key, const glm::vec4& color = glm::vec4(0.f, 0.f, 0.f, 1.f), GLenum filterMin = GL_LINEAR_MIPMAP_LINEAR, GLenum filterMag = GL_LINEAR, GLint wrap = GL_REPEAT, int comp = STBI_rgb_alpha);

		// Returns default black texture
		std::string Default();

		// Returns texture with a specified key
		std::shared_ptr<Texture2D> GetTexture(const std::string& key) const;

		// Deletes all textures
		void ClearTextureCache();

		// Get/Set
		int CacheSize() { return TextureCache.size(); }

	private:

		// Data
		std::map<std::string, std::shared_ptr<Texture2D>> TextureCache;

		Textures2D() {} // Private constructor to make class singleton
		Textures2D(const Textures2D&) = delete; // No copy constructor allowed
		void operator=(const Textures2D&) = delete; // No copy assignment allowed

		// Helpers
		bool ContainsTexture(const std::string& key) const;
	};
} // namespace X3