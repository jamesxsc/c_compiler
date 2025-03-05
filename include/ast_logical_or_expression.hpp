#pragma once

#include "ast_expression_base.hpp"
#include "ast_logical_and_expression.hpp"

namespace ast {

class LogicalOrExpression;
using LogicalOrExpressionPtr = std::unique_ptr<const LogicalOrExpression>;

class LogicalOrExpression : public ExpressionBase {
public:
    Type GetType(Context &context) const override;

    LogicalOrExpression(LogicalOrExpressionPtr left, LogicalAndExpressionPtr right)
        : left_(std::move(left))
        , right_(std::move(right))
    {
    }

    explicit LogicalOrExpression(LogicalAndExpressionPtr right)
        : left_(nullptr)
        , right_(std::move(right))
    {
    }

    ~LogicalOrExpression();

    void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
    void Print(std::ostream &stream) const override;

private:
    LogicalOrExpressionPtr left_;
    LogicalAndExpressionPtr right_;
};

} // namespace ast
