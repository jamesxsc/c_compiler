#include "ast_bindings.hpp"

namespace ast {

    const Variable &Bindings::Get(const std::string &identifier) const {
        auto it = bindingsMap_.find(identifier);
        assert(it != bindingsMap_.end() && "Variable not found in bindings");
        return *it->second;
    }

    const Variable &Bindings::Insert(const std::string &identifier, Variable &&variable) {
        assert(bindingsMap_.find(identifier) == bindingsMap_.end() && "Variable already exists in bindings");
        if (bindings_.empty()) {
            variable.offset = start_; // Stack pointer is always 16-byte aligned
        } else {
            int lastOffset = bindings_.back()->offset;
            int currentAlignment = variable.type.GetAlignment();
            int newOffset = lastOffset - variable.size;
            int misalignment = -newOffset % currentAlignment;
            if (misalignment != 0) {
                newOffset -= currentAlignment - misalignment; // Insert padding
            }
            variable.offset = newOffset;
        }
        assert(variable.offset > -size_ && "Bindings exceed allocated stack frame size");
        VariablePtr ptr = std::make_shared<Variable>(variable);
        bindingsMap_.emplace(identifier, ptr);
        bindings_.push_back(std::move(ptr));
        return *bindings_.back();
    }

    bool Bindings::Contains(const std::string &identifier) const {
        return bindingsMap_.find(identifier) != bindingsMap_.end();
    }

    const Variable &Bindings::InsertOrOverwrite(const std::string &identifier, Variable &&variable) {
        auto it = bindingsMap_.find(identifier);
        if (it != bindingsMap_.end()) {
            // Don't remove from deque because it will allow that offset region to be overwritten
            bindingsMap_.erase(it);
        }
        // clang-tidy is wrong
        return Insert(identifier, std::move(variable)); // NOLINT(*-move-const-arg)
    }

    bool Bindings::IsArray(const std::string &identifier) const {
        auto it = bindingsMap_.find(identifier);
        assert(it != bindingsMap_.end() && "Variable not found in bindings");
        return it->second->type.IsArray();
    }

}
