#pragma once

#include <bitset>
#include <vector>
#include <string>
#include <unordered_map>
#include "register.hpp"
#include "ast_type_specifier.hpp"


namespace ast {
    // TODO this will need to hold type info to infer size etc
    struct Variable {
        int offset;
        int size;
        Register reg;
    };

    struct StackFrame {
        int size;
        std::unordered_map<std::string, Variable> bindings;
    };

// An object of class Context is passed between ast nodes during compilation.
// This can be used to pass around information about what's currently being
// compiled (e.g. function scope and variable names).
    class Context {
    public:
        Context() : temporaries_(0) {}

        Register AllocateTemporary();

        void FreeTemporary(Register reg);

        StackFrame &CurrentFrame();

        void PushFrame(const StackFrame &frame);

        void PushScope();

        void PopScope(std::ostream &stream);

        void PopFrame();

        std::string MakeLabel(const std::string &prefix);
    private:
        std::bitset<7> temporaries_; // using t0... notation for contiguous numbering
        std::vector<StackFrame> stack_;

        int labelId_{};
    };

} // namespace ast
