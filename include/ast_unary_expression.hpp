#pragma once

#include "ast_expression_base.hpp"
#include "ast_postfix_expression.hpp"
#include "ast_multiplicative_expression.hpp"

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
    explicit UnaryExpression(PostfixExpressionPtr child)
        : child_(std::move(child))
        , op_(UnaryOperator::PostfixPromote)
    {
    }

    UnaryExpression(MultiplicativeExpressionPtr child, UnaryOperator op)
        : child_(std::move(child))
        , op_(op)
    {
    }

    ~UnaryExpression();

    Type GetType(Context &context) const override;
    void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
    void Print(std::ostream &stream) const override;

    [[nodiscard]] std::string GetIdentifier() const;

private:
    ExpressionBasePtr child_;  
    UnaryOperator op_;
};

using UnaryExpressionPtr = std::unique_ptr<const UnaryExpression>;

} // namespace ast
