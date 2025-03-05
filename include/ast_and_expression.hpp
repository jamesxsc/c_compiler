#pragma once

#include "ast_expression_base.hpp"
#include "ast_equality_expression.hpp"

namespace ast {

class AndExpression;
using AndExpressionPtr = std::unique_ptr<const AndExpression>;

class AndExpression : public ExpressionBase {
public:
    AndExpression(EqualityExpressionPtr right)
        : left_(nullptr), right_(std::move(right))
    {
    }

    AndExpression(AndExpressionPtr left, EqualityExpressionPtr right)
        : left_(std::move(left)), right_(std::move(right))
    {
    }

    ~AndExpression();

    void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
    void Print(std::ostream &stream) const override;
    Type GetType(Context&) const override;

private:
    AndExpressionPtr left_;
    EqualityExpressionPtr right_;
};

} // namespace ast
