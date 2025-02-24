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

    StackFrame &Context::CurrentFrame() {
        if (stack_.empty()) {
            throw std::runtime_error("No stack frame");
        }

        return stack_.back();
    }

    void Context::PushFrame(const StackFrame &frame) {
        stack_.push_back(frame);
    }

    std::string Context::MakeLabel(const std::string &prefix) {
        std::string label{prefix};
        label.push_back('_');
        label.append(std::to_string(labelId_++));
        return label;
    }

} // namespace ast
