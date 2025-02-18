#pragma once

#include "ast_expression.hpp"

namespace ast {

    class MultiplicativeExpression : public Expression
    {
    public:
        MultiplicativeExpression(ExpressionPtr left, ExpressionPtr right)
            : left_(std::move(left)), right_(std::move(right)) {};
        // TODO support other constructors inc. single unary/child
        // TODO operator enum?
        void EmitRISC(std::ostream &stream, Context &context) const override;
        void Print(std::ostream &stream) const override;
    private:
        ExpressionPtr left_;
        ExpressionPtr right_;
    };

} // namespace ast
