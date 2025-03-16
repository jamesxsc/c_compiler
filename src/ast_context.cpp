#include <stdexcept>
#include <iostream>
#include <cassert>
#include "ast_context.hpp"

namespace ast {

    const Variable &Bindings::Get(const std::string &identifier) const {
        auto it = bindingsMap_.find(identifier);
        assert(it != bindingsMap_.end() && "Variable not found in bindings");
        return *it->second;
    }

    const Variable &Bindings::Insert(const std::string &identifier, Variable &&variable) {
        assert(bindingsMap_.find(identifier) == bindingsMap_.end() && "Variable already exists in bindings");
        if (bindings_.empty()) {
            variable.offset = start_;
        } else {
            variable.offset = bindings_.back()->offset - variable.size;
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

    static int temporaries = 0;

    // We don't free temporaries automatically between functions - we assume there are no leaked register allocs
    Register Context::AllocateTemporary(bool forFloat) {
        ++temporaries;
        if (forFloat) {
            for (size_t i = 0; i < floatTemporaries_.size(); i++) {
                if (!floatTemporaries_.test(i)) {
                    floatTemporaries_.set(i);
                    return FloatTemporaryAtIndex(static_cast<int>(i));
                }
            }
        } else {
            for (size_t i = 0; i < integerTemporaries_.size(); i++) {
                if (!integerTemporaries_.test(i)) {
                    integerTemporaries_.set(i);
                    return IntegerTemporaryAtIndex(static_cast<int>(i));
                }
            }
        }
        throw std::runtime_error("Out of temporaries");
    }

    void Context::FreeTemporary(Register reg) {
        if (IsFloatRegister(reg)) {
            int index = IndexOfFloatTemporary(reg);
            assert(index != -1 && "Attempted to free a non-temporary register");
            if (!floatTemporaries_.test(index))
                std::cerr << "Warning: freeing already free temporary" << std::endl;
            floatTemporaries_.reset(index);
        } else {
            int index = IndexOfIntegerTemporary(reg);
            assert(index != -1 && "Attempted to free a non-temporary register");
            if (!integerTemporaries_.test(index))
                std::cerr << "Warning: freeing already free temporary" << std::endl;
            integerTemporaries_.reset(index);
        }
        --temporaries;
    }

    Context::~Context() {
        if (temporaries != 0) {
            std::cerr << "Warning: " << temporaries << " temporary registers were not freed" << std::endl;
        }
    }

    Register Context::AllocatePersistent(bool forFloat) {
        // todo dont always use the same register or we will be bashing s0 s1 etc
        if (forFloat) {
            for (size_t i = 0; i < floatPersistent_.size(); i++) {
                if (!floatPersistent_.test(i)) {
                    floatPersistent_.set(i);
                    if (!stack_.empty())
                        stack_.back().usedFloatPersistentRegisters.set(
                                i); // Note that it needs to be saved by the current function
                    return FloatPersistentAtIndex(static_cast<int>(i));
                }
            }
        } else {
            // Start at 1 to avoid s0 - frame pointer
            for (size_t i = 1; i < integerPersistent_.size(); i++) {
                if (!integerPersistent_.test(i)) {
                    integerPersistent_.set(i);
                    if (!stack_.empty())
                        stack_.back().usedIntegerPersistentRegisters.set(
                                i); // Note that it needs to be saved by the current function
                    return IntegerPersistentAtIndex(static_cast<int>(i));
                }
            }
        }
        throw std::runtime_error("Out of persistent registers");
    }

    void Context::FreePersistent(Register reg) {
        if (IsFloatRegister(reg)) {
            int index = IndexOfFloatPersistent(reg);
            assert(index != -1 && "Attempted to free a non-persistent register");
            if (!floatPersistent_.test(index))
                std::cerr << "Warning: freeing already free persistent" << std::endl;
            floatPersistent_.reset(index);
        } else {
            int index = IndexOfIntegerPersistent(reg);
            assert(index != -1 && "Attempted to free a non-persistent register");
            if (!integerPersistent_.test(index))
                std::cerr << "Warning: freeing already free persistent" << std::endl;
            integerPersistent_.reset(index);
        }
    }

    StackFrame &Context::CurrentFrame() {
        if (stack_.empty()) {
            throw std::runtime_error("No stack frame");
        }
        return stack_.back();
    }

    void Context::PushFrame(StackFrame &&frame) {
        stack_.push_back(std::move(frame));
    }

    void Context::PopScope(std::ostream &stream) {
        assert(stack_.size() >= 2 && "Attempted to pop scope from empty stack");

        // Any persistent registers used in the scope need to be saved back to the function root frame
        stack_.end()[-2].usedIntegerPersistentRegisters |= CurrentFrame().usedIntegerPersistentRegisters;

        stack_.pop_back();
    }

    void Context::PopFrame() {
        assert(!stack_.empty() && "Attempted to pop frame from empty stack");
        stack_.pop_back();
    }

    void Context::PushScope() {
        assert(!stack_.empty() && "Attempted to push scope without frame on stack");
        stack_.push_back(stack_.back()); // Makes a copy
    }

    void Context::InsertFunction(const std::string &identifier, Function &&function) {
        functions_.emplace(identifier, function);
    }

    const Function &Context::GetFunction(const std::string &identifier) const {
        auto it = functions_.find(identifier);
        assert(it != functions_.end() && "Function not found in context");
        return it->second;
    }

    std::string Context::MakeLabel(const std::string &prefix) {
        std::string label{prefix};
        label.push_back('_');
        label.append(std::to_string(labelId_++));
        return label;
    }

    // todo make it const if we can be bothered to get a const frame and isarray
    bool Context::IsGlobal(const std::string &identifier) {
        bool isGlobal = globals_.find(identifier) != globals_.end();
        if (!isGlobal) return false;

        // Shadowing check
        return !(!stack_.empty() && CurrentFrame().bindings.Contains(identifier));
    }

    bool Context::IsArray(const std::string &identifier) {
        // I like how we call is global first here, because it guarantees we are checking the correct one (shadowing)
        if (IsGlobal(identifier)) {
            auto it = globals_.find(identifier);
            return it != globals_.end() && it->second.IsArray();
        } else {
            return CurrentFrame().bindings.IsArray(identifier);
        }
    }

    void Context::InsertGlobal(const std::string &identifier, TypeSpecifier type) {
        globals_.emplace(identifier, type);
    }

    std::ostream &Context::DeferredRISC() {
        return deferredRISC_;
    }

    void Context::EmitDeferredRISC(std::ostream &stream) {
        stream << deferredRISC_.rdbuf();
    }

    TypeSpecifier Context::GetGlobalType(const std::string &identifier) const {
        auto itVar = globals_.find(identifier);
        if (itVar != globals_.end()) return itVar->second;
        throw std::runtime_error("Context::GetGlobalType Global not found in context");
    }

    void Context::InsertGlobalEnum(const std::string &identifier, const std::map<std::string, int> &values) {
        globalEnums_.Insert(identifier, values);
    }

    // Some duplication in finding where it is but it's ok
    int Context::LookupEnum(const std::string &identifier) {
        // Try in scope first
        if (!stack_.empty()) {
            if (CurrentFrame().enums.Contains(identifier)) {
                return CurrentFrame().enums.Lookup(identifier);
            }
        }
        return globalEnums_.Lookup(identifier); // Will throw if not found
    }

    bool Context::IsEnum(const std::string &identifier) {
        if (!stack_.empty()) {
            if (CurrentFrame().enums.Contains(identifier)) {
                return true;
            }
        }
        return globalEnums_.Contains(identifier);
    }

    void Context::InsertGlobalStruct(const std::string &identifier,
                                     const std::vector<std::pair<std::string, TypeSpecifier>> &members) {
        globalStructs_.Insert(identifier, members);
    }

    std::vector<std::pair<std::string, TypeSpecifier>> Context::GetStruct(const std::string &identifier) {
        // Try in local scope first
        if (!stack_.empty()) {
            if (CurrentFrame().structs.Contains(identifier)) {
                return CurrentFrame().structs.GetStruct(identifier);
            }
        }
        return globalStructs_.GetStruct(identifier); // Will throw if not found
    }

    bool Context::IsStruct(const std::string &identifier) {
        if (!stack_.empty()) {
            if (CurrentFrame().structs.Contains(identifier)) {
                return true;
            }
        }
        return globalStructs_.Contains(identifier);
    }

    void Enums::Insert(const std::string &identifier, const std::map<std::string, int> &values) {
        // Overwrite if already exists
        if (enums_.find(identifier) != enums_.end()) {
            enums_.erase(identifier);
        }
        enums_.emplace(identifier, values);
        for (const auto &pair: values) {
            if (lookup_.find(pair.first) != lookup_.end()) {
                lookup_.erase(pair.first);
            }
            lookup_.emplace(pair.first, pair.second);
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

} // namespace ast
