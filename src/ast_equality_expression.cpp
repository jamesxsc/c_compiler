#include "ast_equality_expression.hpp"

namespace ast {

    void EqualityExpression::EmitRISC(std::ostream &stream, Context &context, int destReg) const {
        switch (op_) {
            case EqualityOperator::Equality: {
                int leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);
                int rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);
                stream << "sub x" << destReg << ",t" << leftReg << ",t" << rightReg << std::endl;
                stream << "seqz x" << destReg << ",x" << destReg << std::endl;
                stream << "andi x" << destReg << ",x" << destReg << ",0xff" << std::endl;
                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }
            case EqualityOperator::Inequality: {
                int leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);
                int rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);
                stream << "sub x" << destReg << ",t" << leftReg << ",t" << rightReg << std::endl;
                stream << "snez x" << destReg << ",x" << destReg << std::endl;
                stream << "andi x" << destReg << ",x" << destReg << ",0xff" << std::endl;
                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }
            case EqualityOperator::RelationalPromote: {
                right_->EmitRISC(stream, context, destReg);
                break;
            }
        }
    }

    void EqualityExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr) {
            left_->Print(stream);
        }
        switch (op_) {
            case EqualityOperator::Equality: {
                stream << " == ";
                break;
            }
            case EqualityOperator::Inequality: {
                stream << " != ";
                break;
            }
            case EqualityOperator::RelationalPromote: {
                break;
            }
        }
        right_->Print(stream);
    }

}
