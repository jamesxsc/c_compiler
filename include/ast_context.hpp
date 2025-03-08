#pragma once

#include <bitset>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <deque>
#include <optional>
#include "register.hpp"
#include "ast_type_specifier.hpp"


namespace ast {
    struct Variable {
        int offset{0};
        int size;
        Register reg{Register::zero}; // probably avoid use
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

        const Variable &Get(const std::string &identifier) const;

        const Variable &Insert(const std::string &identifier, Variable variable);

        // This is for in a scope - removes old and reallocated because may be a different type/size
        const Variable &InsertOrOverwrite(const std::string &identifier, Variable variable);

    private:
        int size_;
        int start_; // Don't overwrite saved ra and s... registers
        std::deque<VariablePtr> bindings_;
        std::unordered_map<std::string, const VariablePtr> bindingsMap_;
    };

    struct StackFrame {
        int size;
        Bindings bindings;
        std::bitset<12> usedPersistentRegisters{1}; // s0 is always used
        // todo consider a return label to aovid multiple return instruction sequences
        std::optional<std::string> breakLabel{std::nullopt};
        std::optional<std::string> continueLabel{std::nullopt};
    };

    class Context {
    public:
        Context() : temporaries_(0) {}

        ~Context();

        Register AllocateTemporary();

        Register AllocatePersistent();

        void FreeTemporary(Register reg);

        void FreePersistent(Register reg);

        StackFrame &CurrentFrame();

        void PushFrame(StackFrame &&frame);

        void PushScope();

        void PopScope(std::ostream &stream);

        void PopFrame();

        void InsertFunction(const std::string &identifier, Function &&function);

        const Function &GetFunction(const std::string &identifier) const;

        std::string MakeLabel(const std::string &prefix);

    private:
        std::bitset<7> temporaries_; // using t0... notation for contiguous numbering
        std::bitset<12> persistent_; // using s0... notation for contiguous numbering

        std::vector<StackFrame> stack_;
        std::unordered_map<std::string, Function> functions_;

        int labelId_{};
    };

} // namespace ast
