#pragma once

#include "ast_expression_base.hpp"
#include "ast_conditional_expression.hpp"
#include "ast_expression.hpp"

namespace ast {

    class ConstantExpression : public ExpressionBase {
    public:
        explicit ConstantExpression(ConditionalExpressionPtr expression) : expression_(std::move(expression)) {};

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] int Evaluate() const override;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        TypeSpecifier GetType(Context &context) const override;

        [[nodiscard]] int GetGlobalValue() const override;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;

    private:
        ConditionalExpressionPtr expression_;
    };
    using ConstantExpressionPtr = std::unique_ptr<const ConstantExpression>;

}
