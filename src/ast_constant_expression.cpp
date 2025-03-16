#include "ast_constant_expression.hpp"

namespace ast {

    void ConstantExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        expression_->EmitRISC(stream, context, destReg);
    }

    void ConstantExpression::Print(std::ostream &stream) const {
        expression_->Print(stream);
    }

    bool ConstantExpression::ContainsFunctionCall() const {
        return expression_->ContainsFunctionCall();
    }

    TypeSpecifier ConstantExpression::GetType(Context &context) const {
        return expression_->GetType(context);
    }

    std::string ConstantExpression::GetGlobalIdentifier() const {
        return expression_->GetGlobalIdentifier();
    }

    int ConstantExpression::Evaluate(Context &context) const {
        return expression_->Evaluate(context);
    }
}
