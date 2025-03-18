#pragma once

#include <bitset>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <set>
#include <map>
#include <memory>
#include <deque>
#include <optional>
#include "register.hpp"
#include "ast_type_specifier.hpp"
#include "ast_bindings.hpp"
#include "ast_enums.hpp"
#include "ast_structs.hpp"

namespace ast {

    struct StackFrame {
        int size;
        // Inefficient but would be effort to change
        Bindings bindings;
        Enums enums{};
        Structs structs{};

        TypeSpecifier returnType{TypeSpecifier::VOID};

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

        TypeSpecifier ResolveTypeAlias(std::vector<TypeSpecifier> specifiers);

        bool SetEmitLHS(bool emitLHS); // Returns the old value for restoring

        [[nodiscard]] bool EmitLHS();

        bool dereference{false};

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

        bool emitLHS_{false};

        int labelId_{}; // Instance counter for unique labels
    };

} // namespace ast
