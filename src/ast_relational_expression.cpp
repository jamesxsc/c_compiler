#include "ast_relational_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void RelationalExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (op_ == RelationalOperator::ShiftPromote) {
            right_->EmitRISC(stream, context, destReg);
            return;
        }

        bool leftStored = right_->ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent() : context.AllocateTemporary();
        left_->EmitRISC(stream, context, leftReg);
        Register rightReg = context.AllocateTemporary();
        right_->EmitRISC(stream, context, rightReg);
        switch (op_) {
            case RelationalOperator::LessThan:
               stream << "slt " << destReg << "," << leftReg << "," << rightReg << std::endl;
                break;
            case RelationalOperator::GreaterThan:
                stream << "sgt " << destReg << "," << leftReg << "," << rightReg << std::endl;
                break;
            case RelationalOperator::LessThanOrEqual:
                stream << "sgt " << destReg << "," << leftReg << "," << rightReg << std::endl;
                stream << "seqz " << destReg << "," << destReg << std::endl;
                stream << "andi " << destReg << "," << destReg << ",0xff" << std::endl;
                break;
            case RelationalOperator::GreaterThanOrEqual:
                stream << "sle " << destReg << "," << leftReg << "," << rightReg << std::endl;
                stream << "seqz " << destReg << "," << destReg << std::endl;
                stream << "andi " << destReg << "," << destReg << ",0xff" << std::endl;
                break;
            case RelationalOperator::ShiftPromote: // Should never reach here
                break;
        }
        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg);
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

    TypeSpecifier RelationalExpression::GetType(Context &context) const {
        return right_->GetType(context);
    }

    bool RelationalExpression::ContainsFunctionCall() const {
        if (op_ == RelationalOperator::ShiftPromote) {
            return right_->ContainsFunctionCall();
        } else {
            return (left_ != nullptr && left_->ContainsFunctionCall()) || right_->ContainsFunctionCall();
        }
    }

    std::string RelationalExpression::GetGlobalIdentifier() const {
        return right_->GetGlobalIdentifier();
    }

    int RelationalExpression::GetGlobalValue() const {
        return right_->GetGlobalValue();
    }

}
