#include "ast_logical_or_expression.hpp"

namespace ast {

    void LogicalOrExpression::EmitRISC(std::ostream &stream, Context &context, int destReg) const {
        if (left_ == nullptr) { // Promotion from logical and expression
            right_->EmitRISC(stream, context, destReg);
        } else {
            // TODO operation depends on types - add a switch on child type(s?)
            int leftReg = context.AllocateTemporary();
            left_->EmitRISC(stream, context, leftReg);
            stream << "bne t" << leftReg << ",zero,.L2" << std::endl;
            context.FreeTemporary(leftReg);
            int rightReg = context.AllocateTemporary();
            right_->EmitRISC(stream, context, rightReg);
            stream << "beq t" << rightReg << ",zero,.L3" << std::endl;
            context.FreeTemporary(rightReg);
            stream << ".L2:" << std::endl;
            stream << "li x" << destReg << ",1" << std::endl;
            stream << "j .L4" << std::endl;
            stream << ".L3:" << std::endl;
            stream << "li x" << destReg << ",0" << std::endl;
            stream << ".L4:" << std::endl;
            // TODO Need context make label unique etc.
        }
    }

    void LogicalOrExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr) {
            left_->Print(stream);
            stream << " || ";
        }
        right_->Print(stream);
    }

}
