#pragma once

// STL
#include <string>
#include <vector>
#include <memory>

namespace X3
{
	// Keeps track of all object names
	class Names
	{
	public:

		// Gets one and only instance
		static Names& Inst();

		// Tries to insert new name
		std::string Insert(const std::string& key);

		// Deletes names
		void ClearNameCache();

	private:
		Names() {} // Private constructor to make class singleton
		Names(const Names&) = delete; // No copy constructor allowed
		void operator=(const Names&) = delete; // No copy assignment allowed

		bool ContainsName(const std::string& name);

		std::vector<std::string> NameCache; // Name cache - stores object names in app
	};
} // namespace X3