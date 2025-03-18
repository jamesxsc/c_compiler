#include "ast_structs.hpp"

#include "ast_type_specifier.hpp"

namespace ast {

    void
    Structs::Insert(const std::string &identifier, const std::vector<std::pair<std::string, TypeSpecifier>> &members) {
        // Overwrite if already exists
        if (structs_.find(identifier) != structs_.end()) {
            structs_.erase(identifier);
        }
        structs_.emplace(identifier, members);
    }

    std::vector<std::pair<std::string, TypeSpecifier>> Structs::GetStruct(const std::string &identifier) const {
        auto it = structs_.find(identifier);
        if (it != structs_.end()) return it->second;
        throw std::runtime_error("Structs::GetStruct Struct not found in context");
    }

    bool Structs::Contains(const std::string &identifier) const {
        return structs_.find(identifier) != structs_.end();
    }

}
