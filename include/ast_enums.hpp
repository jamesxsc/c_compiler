#pragma once

#include <map>
#include <string>
#include <unordered_map>

namespace ast {

    // This probably overkill class is in case we need to iterate over enumerators anywhere
    class Enums {
    public:
        void Insert(const std::string &identifier, const std::map<std::string, int> &values);

        [[nodiscard]] std::map<std::string, int> GetEnum(const std::string &identifier) const;

        int Lookup(const std::string &identifier) const;

        bool Contains(const std::string &identifier) const;

    private:
        std::unordered_map<std::string, std::map<std::string, int>> enums_{};
        std::unordered_map<std::string, int> lookup_{};
    };

}
