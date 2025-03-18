#include "ast_argument_expression_list.hpp"

namespace ast {

    void ArgumentExpressionList::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (arguments_.size() > 8) {
            std::cerr << "More than 8 arguments in a function call is unsupported" << std::endl;
            // todo support this and equiv in parameter list
            exit(1);
        } else {
            Register firstIntegerReg = Register::a0;
            Register firstFloatReg = Register::fa0;
            // todo unfold structs, really need to do it here annoyingly
            for (const auto &argument: arguments_) {
                if (argument->GetType(context) == TypeSpecifier::FLOAT || argument->GetType(context) == TypeSpecifier::DOUBLE) {
                    argument->EmitRISC(stream, context, firstFloatReg);
                    firstFloatReg = static_cast<Register>(static_cast<int>(firstFloatReg) + 1);
                } else {
                    argument->EmitRISC(stream, context, firstIntegerReg);
                    firstIntegerReg = static_cast<Register>(static_cast<int>(firstIntegerReg) + 1);
                }
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
