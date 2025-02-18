#pragma once

#include "ast_expression.hpp"
#include "ast_primary_expression.hpp"

namespace ast {

    class PostfixExpression : public Expression
    {
    public:
        PostfixExpression(ExpressionPtr child) : child_(std::move(child)) {};
        // TODO support other constructors

        void EmitRISC(std::ostream &stream, Context &context) const override;
        void Print(std::ostream &stream) const override;
    private:
        ExpressionPtr child_;
    };

} // namespace ast
