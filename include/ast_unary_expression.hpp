#pragma once

#include "ast_expression_base.hpp"
#include "ast_postfix_expression.hpp"

namespace ast {

    enum class UnaryOperator {
        PostfixPromote,
        Increment,
        Decrement,
        SizeofUnary,
        SizeofType,

        AddressOf,
        Dereference,
        Plus,
        Minus,
        BitwiseNot,
        LogicalNot
    };

    class UnaryExpression : public ExpressionBase
    {
    public:
        UnaryExpression(PostfixExpressionPtr child) : child_(std::move(child)), op_(UnaryOperator::PostfixPromote) {};
        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;

    private:
        PostfixExpressionPtr child_;
        UnaryOperator op_;
    };

    using UnaryExpressionPtr = std::unique_ptr<const UnaryExpression>;

} // namespace ast
