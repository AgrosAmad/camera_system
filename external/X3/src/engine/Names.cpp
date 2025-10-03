#include <Names.h>

namespace X3
{
    Names& Names::Inst()
    {
        static Names names;
        return names;
    }

    std::string Names::Insert(const std::string& key)
    {
        // When the key is already in the app, it adds a number until the name is unique
        std::string name = key;
        int counter = 1;
        while (ContainsName(name)) name = key + "_" + std::to_string(counter++);
        NameCache.push_back(name);

        return name;
    }

    void Names::ClearNameCache()
    {
        NameCache.clear();
    }

    bool Names::ContainsName(const std::string& name)
    {
        // Checks if name already in cache
        for (int i = 0; i < NameCache.size(); i++)
        {
            if (NameCache[i] == name) return true;
        }

        return false;
    }
}