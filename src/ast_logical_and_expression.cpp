#include "ast_logical_and_expression.hpp"

namespace ast {

    void LogicalAndExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (left_ == nullptr) { // Promotion from inclusive or expression
            right_->EmitRISC(stream, context, destReg);
        } else {
            // TODO operation depends on types - add a switch on child type(s?)
            // This is for int
            Register leftReg = context.AllocateTemporary();
            left_->EmitRISC(stream, context, leftReg);
            stream << "beq " << leftReg << ",zero,.L2" << std::endl;
            context.FreeTemporary(leftReg);
            Register rightReg = context.AllocateTemporary();
            right_->EmitRISC(stream, context, rightReg);
            stream << "beq " << rightReg << ",zero,.L2" << std::endl;
            context.FreeTemporary(rightReg);
            stream << "li " << destReg << ",1" << std::endl;
            stream << "j .L3" << std::endl;
            stream << ".L2:" << std::endl;
            stream << "li " << destReg << ",0" << std::endl;
            stream << ".L3:" << std::endl;
            // TODO Need context make label unique etc.
        }
    }

    void LogicalAndExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr) {
            left_->Print(stream);
            stream << " && ";
        }
        right_->Print(stream);
    }

}
