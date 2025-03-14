#include "ast_shift_expression.hpp"
#include "ast_type_specifier.hpp"
#include "risc_utils.hpp"

namespace ast {

    // Doesn't support floats, no need to update
    void ShiftExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        switch (op_) {
            case ShiftOperator::AdditivePromote:
                right_->EmitRISC(stream, context, destReg);
                break;
            case ShiftOperator::Left:
                Utils::EmitLeftShift(stream, context, destReg, *left_, *right_);
                break;
            case ShiftOperator::Right:
                Utils::EmitRightShift(stream, context, destReg, *left_, *right_);
                break;
        }
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
