#pragma once

#include "ast_expression.hpp"
#include "ast_postfix_expression.hpp"

namespace ast {

    class UnaryExpression : public Expression
    {
    public:
        UnaryExpression(PostfixExpressionPtr child) : child_(std::move(child)) {

        };
        // TODO support other constructors
        void EmitRISC(std::ostream &stream, Context &context, int destReg) const override;
        void Print(std::ostream &stream) const override;

    private:
        PostfixExpressionPtr child_;
    };

    using UnaryExpressionPtr = std::unique_ptr<const UnaryExpression>;

} // namespace ast
