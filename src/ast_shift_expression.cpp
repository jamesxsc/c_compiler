#include "ast_shift_expression.hpp"

namespace ast {

    void ShiftExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        switch (op_) {
            case ShiftOperator::Left: {
                Register leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);
                Register rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);
                stream << "sll " << destReg << "," << leftReg << "," << rightReg << std::endl;
                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }
            case ShiftOperator::Right: {
                Register leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);
                Register rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);
                // TODO use shift types - logical for unsigned and arithmetic for signed
                stream << "sra " << destReg << "," << leftReg << "," << rightReg << std::endl;
                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }
            case ShiftOperator::AdditivePromote: {
                right_->EmitRISC(stream, context, destReg);
                break;
            }
        }
    }

    void ShiftExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr)
            left_->Print(stream);
        switch (op_) {
            case ShiftOperator::Left:
                stream << " << ";
                break;
            case ShiftOperator::Right:
                stream << " >> ";
                break;
            case ShiftOperator::AdditivePromote:
                break;
        }
        right_->Print(stream);
    }

}
