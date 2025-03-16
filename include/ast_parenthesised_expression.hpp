#pragma once

#include "ast_primary_expression.hpp"
#include "ast_expression.hpp"

namespace ast {

    class ParenthesisedExpression : public PrimaryExpression {
    public:
        explicit ParenthesisedExpression(ExpressionPtr expression) : expression_(std::move(expression)) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] int Evaluate(Context &context) const override;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        TypeSpecifier GetType(Context &context) const override;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;

    private:
        ExpressionPtr expression_;
    };

}
