#pragma once

#include <bitset>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <map>
#include <memory>
#include <deque>
#include <optional>
#include "register.hpp"
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
        // todo we need to get array locations from *(p+1) sorta ops... or handle this arithmetically somehow in assignment
        // I think the easiest way will be pushing down evaluation of ptrs to identifier
        // Then simply use address rather than identifier (although we need a way to reverse look up address to get type etc)
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

    // Again, overkill but may need to include size logic here
    class Structs {
    public:
        void Insert(const std::string &identifier, const std::vector<std::pair<std::string, TypeSpecifier>> &members);

        [[nodiscard]] std::vector<std::pair<std::string, TypeSpecifier>> GetStruct(const std::string &identifier) const;

        bool Contains(const std::string &identifier) const;

    private:
        std::unordered_map<std::string, std::vector<std::pair<std::string, TypeSpecifier>>> structs_{};
    };

    struct StackFrame {
        int size;
        // Inefficient but would be effort to change
        Bindings bindings;
        Enums enums{};
        Structs structs{};

        std::bitset<12> usedIntegerPersistentRegisters{1}; // s0 is always used
        std::bitset<12> usedFloatPersistentRegisters{};

        std::optional<std::string> returnLabel{std::nullopt};
        std::vector<std::string> breakLabel{};
        std::vector<std::string> continueLabel{};
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

        [[nodiscard]] const Function &GetFunction(const std::string &identifier) const;

        std::string MakeLabel(const std::string &prefix);

        [[nodiscard]] bool IsGlobal(const std::string &identifier);

        [[nodiscard]] bool IsArray(const std::string &identifier);

        void InsertGlobal(const std::string &identifier, TypeSpecifier type);

        [[nodiscard]] TypeSpecifier GetGlobalType(const std::string &identifier) const;

        void InsertGlobalEnum(const std::string &identifier, const std::map<std::string, int> &values);

        [[nodiscard]] bool IsEnum(const std::string &identifier);

        [[nodiscard]] int LookupEnum(const std::string &identifier);

        void InsertGlobalStruct(const std::string &identifier, const std::vector<std::pair<std::string, TypeSpecifier>> &members);

        [[nodiscard]] std::vector<std::pair<std::string, TypeSpecifier>> GetStruct(const std::string &identifier);

        [[nodiscard]] bool IsStruct(const std::string &identifier);

        std::ostream &DeferredRISC();

        void EmitDeferredRISC(std::ostream &stream);

        TypeSpecifier UnifyTypes(const TypeSpecifier &a, const TypeSpecifier &b) const;

    private:
        std::bitset<7> integerTemporaries_; // using t0... notation for contiguous numbering
        std::bitset<12> integerPersistent_; // using s0... notation for contiguous numbering
        std::bitset<12> floatTemporaries_; // ft0 ...
        std::bitset<12> floatPersistent_; // fs0 ...

        std::vector<StackFrame> stack_;
        // Globals are not stored on the stack so do not require the bindings class/ offsets
        std::unordered_map<std::string, TypeSpecifier> globals_;
        Enums globalEnums_{};
        Structs globalStructs_{};
        std::unordered_map<std::string, Function> functions_;

        std::stringstream deferredRISC_{};

        int labelId_{}; // Instance counter for unique labels
    };

} // namespace ast
