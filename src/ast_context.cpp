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

    static int temporaries = 0;

    // We don't free temporaries automatically between functions - we assume there are no leaked register allocs
    Register Context::AllocateTemporary() {
        ++temporaries;
        for (size_t i = 0; i < temporaries_.size(); i++) {
            if (!temporaries_.test(i)) {
                temporaries_.set(i);
                return temporaryAtIndex(static_cast<int>(i));
            }
        }
        throw std::runtime_error("Out of temporaries");
    }

    void Context::FreeTemporary(Register reg) {
        int index = indexOfTemporary(reg);
        assert(index != -1 && "Attempted to free a non-temporary register");
        if (!temporaries_.test(index))
            std::cerr << "Warning: freeing already free temporary" << std::endl;
        --temporaries;
        temporaries_.reset(index);
    }

    Context::~Context() {
        if (temporaries != 0) {
            std::cerr << "Warning: " << temporaries << " temporary registers were not freed" << std::endl;
        }
    }

    Register Context::AllocatePersistent() {
        // todo dont always use the same register or we will be bashing s0 s1 etc
        // Start at 1 to avoid s0 - frame pointer
        for (size_t i = 1; i < persistent_.size(); i++) {
            if (!persistent_.test(i)) {
                persistent_.set(i);
                if (!stack_.empty())
                    stack_.back().usedPersistentRegisters.set(
                            i); // Note that it needs to be saved by the current function
                return persistentAtIndex(static_cast<int>(i));
            }
        }
        throw std::runtime_error("Out of persistent registers");
    }

    void Context::FreePersistent(Register reg) {
        int index = indexOfPersistent(reg);
        assert(index != -1 && "Attempted to free a non-persistent register");
        if (!persistent_.test(index))
            std::cerr << "Warning: freeing already free persistent" << std::endl;
        persistent_.reset(index);
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
        stack_.end()[-2].usedPersistentRegisters |= CurrentFrame().usedPersistentRegisters;

        stack_.pop_back();

        // todo do we need to store anything
        // and restore registers? unclear
//        if (stack_.size() >= 2) {
//            int topSize = CurrentFrame().size;
//            int oldSize = stack_.end()[-2].size;
//            stream << "addi sp,sp," << (topSize - oldSize) << std::endl;
//        } else {
//
//            stream << "# PopScope: only one frame on stack, skipping sp restore\n";
//        }
//
//        stack_.pop_back();
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

} // namespace ast
