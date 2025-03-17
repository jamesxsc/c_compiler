#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "ast_type_specifier.hpp"

namespace ast {

    // Again, overkill but may need to include size logic here
    class Structs {
    public:
        void Insert(const std::string &identifier, const std::vector<std::pair<std::string, TypeSpecifier>> &members);

        [[nodiscard]] std::vector<std::pair<std::string, TypeSpecifier>> GetStruct(const std::string &identifier) const;

        bool Contains(const std::string &identifier) const;

    private:
        std::unordered_map<std::string, std::vector<std::pair<std::string, TypeSpecifier>>> structs_{};
    };

}
