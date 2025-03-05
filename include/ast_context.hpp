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
        TypeSpecifier type;
    };

    struct StackFrame {
        int size;
        std::unordered_map<std::string, Variable> bindings;
        std::bitset<12> usedPersistentRegisters{1}; // s0 is always used
    };

    class Context {
    public:
        Context() : temporaries_(0) {}

        Register AllocateTemporary();

        Register AllocatePersistent();

        void FreeTemporary(Register reg);

        void FreePersistent(Register reg);

        StackFrame &CurrentFrame();

        void PushFrame(const StackFrame &frame);

        void PushScope();

        void PopScope(std::ostream &stream);

        void PopFrame();

        std::string MakeLabel(const std::string &prefix);

    private:
        std::bitset<7> temporaries_; // using t0... notation for contiguous numbering
        std::bitset<12> persistent_; // using s0... notation for contiguous numbering

        std::vector<StackFrame> stack_;

        int labelId_{};
    };

} // namespace ast
