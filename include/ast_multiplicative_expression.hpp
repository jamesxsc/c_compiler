#pragma once

#include "ast_expression_base.hpp"
#include <memory>

namespace ast {
class UnaryExpression; 
using UnaryExpressionPtr = std::unique_ptr<const UnaryExpression>;
}

namespace ast {

enum class MultiplicativeOperator {
    UnaryPromote,
    Multiply,
    Divide,
    Modulo
};

class MultiplicativeExpression; 
using MultiplicativeExpressionPtr = std::unique_ptr<const MultiplicativeExpression>;

class MultiplicativeExpression : public ExpressionBase
{
public:
    MultiplicativeExpression(MultiplicativeExpressionPtr left, UnaryExpressionPtr right, MultiplicativeOperator op)
        : left_(std::move(left)), right_(std::move(right)), op_(op) {}

    explicit MultiplicativeExpression(UnaryExpressionPtr right)
        : left_(nullptr), right_(std::move(right)), op_(MultiplicativeOperator::UnaryPromote) {}

    ~MultiplicativeExpression();

    Type GetType(Context &context) const override;
    void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
    void Print(std::ostream &stream) const override;

private:
    MultiplicativeExpressionPtr left_;
    UnaryExpressionPtr right_;
    MultiplicativeOperator op_;
};

} // namespace ast
