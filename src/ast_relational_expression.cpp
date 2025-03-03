#include "ast_relational_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void RelationalExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        switch (op_) {
            case RelationalOperator::LessThan: {
                Register leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);
                Register rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);
                stream << "slt " << destReg << "," << leftReg << "," << rightReg << std::endl;
                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }
            case RelationalOperator::GreaterThan: {
                Register leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);
                Register rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);
                stream << "sgt " << destReg << "," << leftReg << "," << rightReg << std::endl;
                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }
            case RelationalOperator::LessThanOrEqual: {
                Register leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);
                Register rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);
                stream << "sgt " << destReg << "," << leftReg << "," << rightReg << std::endl;
                stream << "seqz " << destReg << "," << destReg << std::endl;
                stream << "andi " << destReg << "," << destReg << ",0xff" << std::endl;
                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }
            case RelationalOperator::GreaterThanOrEqual: {
                Register leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);
                Register rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);
                stream << "sle " << destReg << "," << leftReg << "," << rightReg << std::endl;
                stream << "seqz " << destReg << "," << destReg << std::endl;
                stream << "andi " << destReg << "," << destReg << ",0xff" << std::endl;
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

    Type RelationalExpression::GetType(Context &context) const {  
        return Type(TypeSpecifier::INT, true);
    }

    bool RelationalExpression::ContainsFunctionCall() const {
        if (op_ == RelationalOperator::ShiftPromote) {
            return right_->ContainsFunctionCall();
        } else {
            return (left_ != nullptr && left_->ContainsFunctionCall()) || right_->ContainsFunctionCall();
        }
    }

}
