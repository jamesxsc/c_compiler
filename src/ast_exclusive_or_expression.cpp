#include "ast_exclusive_or_expression.hpp"

namespace ast {

    void ExclusiveOrExpression::EmitRISC(std::ostream &stream, Context &context, int destReg) const {
        if (left_ == nullptr) { // Promotion from and expression
            right_->EmitRISC(stream, context, destReg);
        } else {
            int leftReg = context.AllocateTemporary();
            left_->EmitRISC(stream, context, leftReg);
            int rightReg = context.AllocateTemporary();
            right_->EmitRISC(stream, context, rightReg);
            stream << "xor x" << destReg << ",t" << leftReg << ",t" << rightReg << std::endl;
            context.FreeTemporary(leftReg);
            context.FreeTemporary(rightReg);
        }
    }

    void ExclusiveOrExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr) {
            left_->Print(stream);
            stream << " ^ ";
        }
        right_->Print(stream);

    }
}
