#include <stdexcept>
#include <iostream>
#include "ast_context.hpp"

namespace ast {

    int Context::AllocateTemporary() {
        for (size_t i = 0; i < temporaries_.size(); i++) {
            if (!temporaries_.test(i)) {
                temporaries_.set(i);
                return static_cast<int>(i);
            }
        }
        throw std::runtime_error("Out of temporaries");
    }

    void Context::FreeTemporary(int index) {
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

} // namespace ast
