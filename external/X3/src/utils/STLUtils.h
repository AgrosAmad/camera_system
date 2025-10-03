#pragma once

// STL
#include <vector>
#include <memory>

// Helpers
namespace X3
{
	namespace utils
	{
		// Erase element from vector
		template<class T, class U>
		void EraseFromVector(std::shared_ptr<U> element, std::vector<std::shared_ptr<T>>& vector)
		{
			auto element_cast = std::dynamic_pointer_cast<T>(element);
			auto it = std::find(vector.begin(), vector.end(), element_cast);
			if (it != vector.end()) vector.erase(it);
		}

		// Iterate over a vector and delete expired weak_ptrs
		template<class T>
		void EraseExpiredPointers(std::vector<std::weak_ptr<T>>& vec)
		{
			auto it = vec.begin();
			while (it != vec.end())
			{
				if (it->expired())
				{
					it = vec.erase(it); // Remove expired weak_ptr from the vector
				}
				else {
					++it;
				}
			}
		}
	} // ends namespace Utils
} // ends namespace X3