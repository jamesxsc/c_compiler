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
        if (op_ == ShiftOperator::AdditivePromote) {
            return right_->GetType(context);
        }

        TypeSpecifier leftType = left_->GetType(context);
        if (leftType == TypeSpecifier::UNSIGNED) {
            return TypeSpecifier::UNSIGNED; // Only unsigned won't be promoted because it can't fit in a signed int
        } else {
            return TypeSpecifier::INT;
        }
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

    int ShiftExpression::Evaluate(Context &context) const {
        switch (op_) {
            case ShiftOperator::AdditivePromote:
                return right_->Evaluate(context);
            case ShiftOperator::Left: // May not be watertight if theres something with stupid types
                return left_->Evaluate(context) << right_->Evaluate(context);
            case ShiftOperator::Right:
                return left_->Evaluate(context) >> right_->Evaluate(context);
        }
        throw std::runtime_error("ShiftExpression::Evaluate() reached end of function");
    }

    double ShiftExpression::EvaluateFloat(ast::Context &context) const {
        if (op_ == ShiftOperator::AdditivePromote) {
            return right_->EvaluateFloat(context);
        }

        throw std::runtime_error("ShiftExpression::EvaluateFloat() Cannot shift floats");
    }

}
