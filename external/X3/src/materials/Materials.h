#pragma once

// STL
#include <string>
#include <map>
#include <memory>

// X3
#include <Parameters.h>
#include <Material.h>

namespace X3
{
	// Singleton material manager
	class Materials
	{
	public:

		// Gets the one and only instance of the material manager.
		static Materials& Inst();

		// Common methods
		std::shared_ptr<Material> Load(const std::string& key, MatType type);
		void ClearCache();
		void Update();

		// Get/Set
		std::map<std::string, std::shared_ptr<Material>>& MatCache() { return mMatCache; }
		std::shared_ptr<Material> GetMaterial(const std::string& key) const;
		int CacheSize() { return mMatCache.size(); }

	private:

		// Data
		std::map<std::string, std::shared_ptr<Material>> mMatCache; // material cache

		// Helpers
		Materials() {} // Private constructor to make class singleton
		Materials(const Materials&) = delete; // No copy constructor allowed
		void operator=(const Materials&) = delete; // No copy assignment allowed

		bool ContainsMaterial(const std::string& key) const;
	};
} // namespace X3