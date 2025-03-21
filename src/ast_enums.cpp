#include "ast_enums.hpp"

#include <stdexcept>

namespace ast {

    void Enums::Insert(const std::string &identifier, const std::map<std::string, int> &values) {
        // Overwrite if already exists
        if (enums_.find(identifier) != enums_.end()) {
            enums_.erase(identifier);
        }
        enums_.insert_or_assign(identifier, values);
        Insert(values);
    }

    // Anonymous
    void Enums::Insert(const std::map<std::string, int> &values) {
        for (const auto &pair: values) {
            if (lookup_.find(pair.first) != lookup_.end()) {
                lookup_.erase(pair.first);
            }
            lookup_.insert_or_assign(pair.first, pair.second);
        }
    }

    std::map<std::string, int> Enums::GetEnum(const std::string &identifier) const {
        auto it = enums_.find(identifier);
        if (it != enums_.end()) return it->second;
        throw std::runtime_error("Enums::GetEnum Enum not found in context");
    }

    int Enums::Lookup(const std::string &identifier) const {
        return lookup_.at(identifier);
    }

    bool Enums::Contains(const std::string &identifier) const {
        return lookup_.find(identifier) != lookup_.end();
    }

}
