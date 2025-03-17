#pragma once

#include <deque>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <cassert>
#include "ast_type_specifier.hpp"

namespace ast {

    struct Variable {
        int offset{0};
        int size; // Bytes
        TypeSpecifier type;
    };
    using VariablePtr = std::shared_ptr<const Variable>;

    struct Function {
        std::vector<int> parameterSizes;
        int totalSize;
        TypeSpecifier returnType;
    };

    // I prefer this, but it may be possible to ditch the deque and just store a counter
    class Bindings {
    public:
        Bindings(int size, int start) : size_(size), start_(start) {};

        [[nodiscard]] const Variable &Get(const std::string &identifier) const;

        [[nodiscard]] bool Contains(const std::string &identifier) const;

        // Must pass as rvalue to avoid slicing Array type
        const Variable &Insert(const std::string &identifier, Variable &&variable);

        // This is for in a scope - removes old and reallocated because may be a different type/size
        const Variable &InsertOrOverwrite(const std::string &identifier, Variable &&variable);

        bool IsArray(const std::string &identifier) const;

    private:
        int size_;
        int start_; // Don't overwrite saved ra and s... registers
        std::deque<VariablePtr> bindings_;
        std::unordered_map<std::string, const VariablePtr> bindingsMap_;
    };

}
