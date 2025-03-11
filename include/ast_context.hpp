#pragma once

#include <bitset>
#include <vector>
#include <string>
#include <sstream>
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
        Register reg{Register::zero}; // todo probably avoid use
        TypeSpecifier type;
        bool global{false};
    };

    // Hmmmm, need to think about slicing here. But we have to be able to copy - wait no - we already use shared ptr
    // this might work - where does instantiation go - parameter list, declaration, what is delegated risc-wise to declarator? i.e. what do we need in arraydeclarator
    // ok starting to come together, initdeclarator/declarator/arraydeclarator need methods for array like IsFunction IsPointer
    // parameter list (arrays as function parameters) will be a bitch
    // acessing should be easy-ish. we find this by the array identifier
    // is it an array? ok we need to get the offset of the elem - member function?
    struct Array : Variable {
        Array(TypeSpecifier elementType, int length) :
                Variable({.size = GetTypeSize(elementType) * length, .type = elementType}), length(length) {}

        int length;
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

        bool Contains(const std::string &identifier) const;

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
        std::bitset<12> usedIntegerPersistentRegisters{1}; // s0 is always used
        std::bitset<12> usedFloatPersistentRegisters{};
        // todo consider a return label to aovid multiple return instruction sequences
        std::optional<std::string> breakLabel{std::nullopt};
        std::optional<std::string> continueLabel{std::nullopt};
    };

    class Context {
    public:
        Context() : integerTemporaries_(0), integerPersistent_(1), floatTemporaries_(0), floatPersistent_(0) {}

        ~Context();

        Register AllocateTemporary(bool forFloat = false);

        Register AllocatePersistent(bool forFloat = false);

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

        [[nodiscard]] bool IsGlobal(const std::string &identifier);

        void InsertGlobal(const std::string &identifier, TypeSpecifier type);

        TypeSpecifier GetGlobalType(const std::string &identifier) const;

        std::ostream &DeferredRISC();

        void EmitDeferredRISC(std::ostream &stream);

    private:
        std::bitset<7> integerTemporaries_; // using t0... notation for contiguous numbering
        std::bitset<12> integerPersistent_; // using s0... notation for contiguous numbering
        std::bitset<12> floatTemporaries_; // ft0 ...
        std::bitset<12> floatPersistent_; // fs0 ...

        std::vector<StackFrame> stack_;
        std::unordered_map<std::string, TypeSpecifier> globals_;
        std::unordered_map<std::string, Function> functions_;

        std::stringstream deferredRISC_{};

        int labelId_{}; // Instance counter for unique labels
    };

} // namespace ast
