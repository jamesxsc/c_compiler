#include "ast_type_specifier.hpp"
#include "ast_additive_expression.hpp"
#include "risc_utils.hpp"

namespace ast {

    void AdditiveExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        switch (op_) {
            case AdditiveOperator::MultiplicativePromote:
                right_->EmitRISC(stream, context, destReg);
                break;
            case AdditiveOperator::Add:
                Utils::EmitAddition(stream, context, destReg, *left_, *right_);
                break;
            case AdditiveOperator::Subtract:
                Utils::EmitSubtraction(stream, context, destReg, *left_, *right_);
                break;
        }
    }

    void AdditiveExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr) {
            left_->Print(stream);
        }

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


    TypeSpecifier AdditiveExpression::GetType(Context &context) const {
        // todo technically should be l or r e.g. for float, need some ordering/ranking of types and propagate elsewhere
        return right_->GetType(context);
    }

    bool AdditiveExpression::ContainsFunctionCall() const {
        if (op_ == AdditiveOperator::MultiplicativePromote) {
            return right_->ContainsFunctionCall();
        } else {
            return (left_ != nullptr && left_->ContainsFunctionCall()) || right_->ContainsFunctionCall();
        }
    }

    std::string AdditiveExpression::GetGlobalIdentifier() const {
        return right_->GetGlobalIdentifier();
    }

    int AdditiveExpression::GetGlobalValue() const {
        return right_->GetGlobalValue();
    }

} // namespace ast
