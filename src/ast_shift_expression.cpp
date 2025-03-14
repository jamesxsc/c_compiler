#include "ast_shift_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    // Doesn't support floats, no need to update
    void ShiftExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (op_ == ShiftOperator::AdditivePromote) {
            right_->EmitRISC(stream, context, destReg);
            return;
        }

        bool leftStored = right_->ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent() : context.AllocateTemporary();
        left_->EmitRISC(stream, context, leftReg);
        Register rightReg = context.AllocateTemporary();
        right_->EmitRISC(stream, context, rightReg);
        switch (op_) {
            case ShiftOperator::Left: {
               stream << "sll " << destReg << "," << leftReg << "," << rightReg << std::endl;
                break;
            }
            case ShiftOperator::Right: {
                if (left_->GetType(context).IsSigned())
                    stream << "sra " << destReg << "," << leftReg << "," << rightReg << std::endl;
                else
                    stream << "srl " << destReg << "," << leftReg << "," << rightReg << std::endl;
                break;
            }
            case ShiftOperator::AdditivePromote: // Should never reach here
                break;
        }
        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg);
        context.FreeTemporary(rightReg);
    }

    void ShiftExpression::Print(std::ostream &stream) const {
        if (left_) {
            left_->Print(stream);
        }
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

    TypeSpecifier ShiftExpression::GetType(Context &context) const {
        return right_->GetType(context);
    }

    bool ShiftExpression::ContainsFunctionCall() const {
        if (op_ == ShiftOperator::AdditivePromote) {
            return right_->ContainsFunctionCall();
        } else {
            return (left_ != nullptr && left_->ContainsFunctionCall()) || right_->ContainsFunctionCall();
        }
    }

    std::string ShiftExpression::GetGlobalIdentifier() const {
        return right_->GetGlobalIdentifier();
    }

    int ShiftExpression::GetGlobalValue() const {
        return right_->GetGlobalValue();
    }

}
