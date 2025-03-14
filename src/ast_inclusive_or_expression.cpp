#include "ast_inclusive_or_expression.hpp"
#include "ast_type_specifier.hpp"
#include "risc_utils.hpp"

namespace ast {

    void InclusiveOrExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (left_ == nullptr) { // Promotion from exclusive or expression
            right_->EmitRISC(stream, context, destReg);
        } else {
            Utils::EmitBitwiseOr(stream, context, destReg, *left_, *right_);
        }
    }

    void InclusiveOrExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr) {
            left_->Print(stream);
            stream << " | ";
        }
        right_->Print(stream);

    }

    TypeSpecifier InclusiveOrExpression::GetType(Context& context) const {
        return right_->GetType(context);
    }

    bool InclusiveOrExpression::ContainsFunctionCall() const {
        return (left_ != nullptr && left_->ContainsFunctionCall()) || right_->ContainsFunctionCall();
    }

    std::string InclusiveOrExpression::GetGlobalIdentifier() const {
        return right_->GetGlobalIdentifier();
    }

    int InclusiveOrExpression::GetGlobalValue() const {
        return right_->GetGlobalValue();
    }

}
