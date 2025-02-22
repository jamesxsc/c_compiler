#include "ast_relational_expression.hpp"

namespace ast {

    void RelationalExpression::EmitRISC(std::ostream &stream, Context &context, int destReg) const {
        switch (op_) {
            case RelationalOperator::LessThan: {
                int leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);
                int rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);
                stream << "slt x" << destReg << ",t" << leftReg << ",t" << rightReg << std::endl;
                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }
            case RelationalOperator::GreaterThan: {
                int leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);
                int rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);
                stream << "sgt x" << destReg << ",t" << leftReg << ",t" << rightReg << std::endl;
                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }
            case RelationalOperator::LessThanOrEqual: {
                int leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);
                int rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);
                // Achieve by inverting greater than result
                stream << "sgt x" << destReg << ",t" << leftReg << ",t" << rightReg << std::endl;
                stream << "seqz x" << destReg << ",x" << destReg << std::endl;
                stream << "andi x" << destReg << ",x" << destReg << ",0xff" << std::endl;
                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }
            case RelationalOperator::GreaterThanOrEqual: {
                int leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);
                int rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);
                // Achieve by inverting less than result
                stream << "sle x" << destReg << ",t" << leftReg << ",t" << rightReg << std::endl;
                stream << "seqz x" << destReg << ",x" << destReg << std::endl;
                stream << "andi x" << destReg << ",x" << destReg << ",0xff" << std::endl;
                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }
            case RelationalOperator::ShiftPromote: {
                right_->EmitRISC(stream, context, destReg);
                break;
            }
        }
    }

    void RelationalExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr)
            left_->Print(stream);
        switch (op_) {
            case RelationalOperator::LessThan:
                stream << " < ";
                break;
            case RelationalOperator::GreaterThan:
                stream << " > ";
                break;
            case RelationalOperator::LessThanOrEqual:
                stream << " <= ";
                break;
            case RelationalOperator::GreaterThanOrEqual:
                stream << " >= ";
                break;
            case RelationalOperator::ShiftPromote:
                break;
        }
        right_->Print(stream);
    }

}
