#include "ast_parenthesised_expression.hpp"

namespace ast {

    void ParenthesisedExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        expression_->EmitRISC(stream, context, destReg);
    }

    void ParenthesisedExpression::Print(std::ostream &stream) const {
        stream << "(";
        expression_->Print(stream);
        stream << ")";
    }

    bool ParenthesisedExpression::ContainsFunctionCall() const {
        return expression_->ContainsFunctionCall();
    }

    TypeSpecifier ParenthesisedExpression::GetType(Context &context) const {
        return expression_->GetType(context);
    }

    std::string ParenthesisedExpression::GetGlobalIdentifier() const {
        return expression_->GetGlobalIdentifier();
    }

    int ParenthesisedExpression::Evaluate(Context &context) const {
        return expression_->Evaluate(context);
    }
}
