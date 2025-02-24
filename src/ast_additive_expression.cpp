#include "ast_additive_expression.hpp"

namespace ast {

    void AdditiveExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        switch (op_) {
            // Blocks to allow reuse of variable names
            case AdditiveOperator::Add: {
                Register leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);
                Register rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);
                stream << "add " << destReg << "," << leftReg << "," << rightReg << std::endl;
                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }
            case AdditiveOperator::Subtract: {
                // TODO implement subtraction
                // TODO test associativity of subtraction wrt addition
                break;
            }
            case AdditiveOperator::MultiplicativePromote: {
                right_->EmitRISC(stream, context, destReg);
                break;
            }
        }
    }

    void AdditiveExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr)
            left_->Print(stream);
        switch (op_) {
            case AdditiveOperator::Add:
                stream << " + ";
                break;
            case AdditiveOperator::Subtract:
                stream << " - ";
                break;
            case AdditiveOperator::MultiplicativePromote:
                break;
        }
        right_->Print(stream);
    }

}
