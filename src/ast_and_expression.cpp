#include "ast_and_expression.hpp"
#include "ast_type_specifier.hpp"
#include "risc_utils.hpp"

namespace ast {

    void AndExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (left_ == nullptr) { // Promotion from equality expression
            right_->EmitRISC(stream, context, destReg);
        } else {
            Utils::EmitBitwiseAnd(stream, context, destReg, *left_, *right_);
        }
    }

    void AndExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr) {
            left_->Print(stream);
            stream << " & ";
        }
        right_->Print(stream);
    }

    TypeSpecifier AndExpression::GetType(Context &context) const {
        if (left_ == nullptr) {
            return right_->GetType(context);
        }

        return Utils::BinaryResultType(left_->GetType(context), right_->GetType(context));
    }

    bool AndExpression::ContainsFunctionCall() const {
        return (left_ != nullptr && left_->ContainsFunctionCall()) || right_->ContainsFunctionCall();
    }

    std::string AndExpression::GetGlobalIdentifier() const {
        return right_->GetGlobalIdentifier();
    }

    int AndExpression::Evaluate(Context &context) const {
        if (left_ == nullptr) {
            return right_->Evaluate(context);
        }

        return left_->Evaluate(context) & right_->Evaluate(context);
    }

}
