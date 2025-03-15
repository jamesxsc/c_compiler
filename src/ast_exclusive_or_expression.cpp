#include "ast_exclusive_or_expression.hpp"
#include "ast_type_specifier.hpp"
#include "risc_utils.hpp"

namespace ast {

    void ExclusiveOrExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (left_ == nullptr) { // Promotion from and expression
            right_->EmitRISC(stream, context, destReg);
        } else {
            Utils::EmitBitwiseXor(stream, context, destReg, *left_, *right_);
        }
    }

    void ExclusiveOrExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr) {
            left_->Print(stream);
            stream << " ^ ";
        }
        right_->Print(stream);

    }


    TypeSpecifier ExclusiveOrExpression::GetType(Context& context) const {
        if (left_ == nullptr) {
            return right_->GetType(context);
        }

        return Utils::BinaryResultType(left_->GetType(context), right_->GetType(context));
    }

    bool ExclusiveOrExpression::ContainsFunctionCall() const {
        return (left_ != nullptr && left_->ContainsFunctionCall()) || right_->ContainsFunctionCall();
    }

    std::string ExclusiveOrExpression::GetGlobalIdentifier() const {
        return right_->GetGlobalIdentifier();
    }

    int ExclusiveOrExpression::GetGlobalValue() const {
        return right_->GetGlobalValue();
    }

    int ExclusiveOrExpression::Evaluate() const {
        if (left_ == nullptr) {
            return right_->Evaluate();
        }

        return left_->Evaluate() ^ right_->Evaluate();
    }

}
