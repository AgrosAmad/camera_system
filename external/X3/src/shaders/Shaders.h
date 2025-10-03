#pragma once

// STL
#include <string>
#include <map>
#include <memory>

// GL
#include <glad/glad.h>

// X3
#include "Shader.h"

namespace X3
{
    // Singleton class that manages and keeps track of all shader programs.
	class Shaders
	{
	public:
		// Gets the one and only instance of the shader program manager.
		static Shaders& Inst();

		// Main-loop methods
		void Update();

		// Creates new shader program and stores it with specified key.
		std::shared_ptr<Shader> Load(const std::string& key, const std::vector<std::string>& shadersFn);

		// Creates new shader program and stores it with specified key (just using vertex and fragment).
		std::shared_ptr<Shader> Load(const std::string& key, const std::string mainPath);

		// Retrieves shader program with a specified key.
		std::shared_ptr<Shader> GetProgram(const std::string& key) const;

		// Gets forward cache
		std::map<std::string, std::shared_ptr<Shader>>& FwdCache() { return mFwdCache; }

		// Deletes all the shader programs loaded and clears the shader program cache.
		void ClearProgramCache();

		// Get/Set
		int CacheSize() { return mFwdCache.size(); }

	private:
		Shaders() {} // Private constructor to make class singleton
		Shaders(const Shaders&) = delete; // No copy constructor allowed
		void operator=(const Shaders&) = delete; // No copy assignment allowed

		std::map<std::string, std::shared_ptr<Shader>> mFwdCache; // Fwd shader program
		
		// Helpers
		bool ContainsShaderProgram(const std::string& key) const;
	};
} // namespace X3