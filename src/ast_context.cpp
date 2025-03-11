#include <stdexcept>
#include <iostream>
#include <cassert>
#include "ast_context.hpp"

namespace ast {

    int Array::ElementOffset(int index) {
        return offset + index * GetTypeSize(type);
    }

    const Variable &Bindings::Get(const std::string &identifier) const {
        auto it = bindingsMap_.find(identifier);
        assert(it != bindingsMap_.end() && "Variable not found in bindings");
        return *it->second;
    }

    const Variable &Bindings::Insert(const std::string &identifier, Variable variable) {
        assert(bindingsMap_.find(identifier) == bindingsMap_.end() && "Variable already exists in bindings");
        if (bindings_.empty()) {
            variable.offset = start_;
        } else {
            variable.offset = bindings_.back()->offset - bindings_.back()->size;
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

    const Variable &Bindings::InsertOrOverwrite(const std::string &identifier, ast::Variable variable) {
        auto it = bindingsMap_.find(identifier);
        if (it != bindingsMap_.end()) {
            // Don't remove from deque because it will allow that offset region to be overwritten
            bindingsMap_.erase(it);
        }
        return Insert(identifier, variable);
    }

    bool Bindings::IsArray(const std::string &identifier) const {
        auto it = bindingsMap_.find(identifier);
        assert(it != bindingsMap_.end() && "Variable not found in bindings");
        return it->second->array;
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

        // todo do we need to store anything
        // and restore registers? unclear
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

    // todo make it const if we can be bothered to get a const frame
    bool Context::IsGlobal(const std::string &identifier) {
        if (!(globals_.find(identifier) != globals_.end())) return false;

        // Shadowing check
        return !(!stack_.empty() && CurrentFrame().bindings.Contains(identifier));
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
        auto it = globals_.find(identifier);
        assert(it != globals_.end() && "Global variable not found in context");
        return it->second;
    }
} // namespace ast
