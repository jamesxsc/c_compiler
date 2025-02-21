#pragma once

#include "ast_expression.hpp"

namespace ast {

    class UnaryExpression : public Expression
    {
    public:
        UnaryExpression(ExpressionPtr child) : child_(std::move(child)) {

        };
        // TODO support other constructors
        void EmitRISC(std::ostream &stream, Context &context, int destReg) const override;
        void Print(std::ostream &stream) const override;

    private:
        ExpressionPtr child_;
    };

} // namespace ast
