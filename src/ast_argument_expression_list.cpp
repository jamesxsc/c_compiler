#include "ast_argument_expression_list.hpp"

namespace ast {

    void ArgumentExpressionList::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (arguments_.size() > 8) {
            std::cerr << "More than 8 arguments in a function call is unsupported" << std::endl;
            exit(1);
        } else {
            Register firstReg = Register::a0;
            for (const auto &argument: arguments_) {
                argument->EmitRISC(stream, context, firstReg);
                firstReg = static_cast<Register>(static_cast<int>(firstReg) + 1);
            }
        }
    }

    void ArgumentExpressionList::Print(std::ostream &stream) const {
        for (auto it = arguments_.begin(); it != arguments_.end(); ++it) {
            (*it)->Print(stream);
            if (it + 1 != arguments_.end()) {
                stream << ", ";
            }
        }
    }

    void ArgumentExpressionList::PushBack(AssignmentExpressionPtr item) {
        arguments_.push_back(std::move(item));
    }

    size_t ArgumentExpressionList::Size() const {
        return arguments_.size();
    }

    std::vector<AssignmentExpressionPtr>::const_iterator ArgumentExpressionList::begin() const {
        return arguments_.begin();
    }

    std::vector<AssignmentExpressionPtr>::const_iterator ArgumentExpressionList::end() const {
        return arguments_.end();
    }

}
