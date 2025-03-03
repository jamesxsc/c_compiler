#include <stdexcept>
#include <iostream>
#include <cassert>
#include "ast_context.hpp"

namespace ast {

    Register Context::AllocateTemporary() {
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
        temporaries_.reset(index);
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

    void Context::PushFrame(const StackFrame &frame) {
        stack_.push_back(frame);
    }

    void Context::PopScope(std::ostream &stream) {

        // TODO FIX
        assert(stack_.size() >= 2 && "Attempted to pop scope from empty stack");

        // todo is this the best way to do this?
        // Any persistent registers used in the scope need to be saved back to the function root frame
        stack_.end()[-2].usedPersistentRegisters |= CurrentFrame().usedPersistentRegisters;

        // todo this needs to be before return (although won't be relevant eventually - stack size will be fixed)
        stream << "addi sp,sp," << CurrentFrame().size - stack_.end()[-2].size << std::endl;
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
        // TODO: If needed, restore saved registers or do other teardown logic
    }

    void Context::PopFrame() {
        assert(!stack_.empty() && "Attempted to pop frame from empty stack");
        stack_.pop_back();
    }

    void Context::PushScope() {
        assert(!stack_.empty() && "Attempted to push scope without frame on stack");
        stack_.push_back(stack_.back()); // Makes a copy
    }

    std::string Context::MakeLabel(const std::string &prefix) {
        std::string label{prefix};
        label.push_back('_');
        label.append(std::to_string(labelId_++));
        return label;
    }

} // namespace ast
