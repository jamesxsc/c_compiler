#pragma once

#include "ast_expression.hpp"

namespace ast {

    enum class MultiplicativeOperator {
        UnaryPromote,
        Multiply,
        Divide,
        Modulo
    };

    class MultiplicativeExpression : public Expression
    {
    public:
        MultiplicativeExpression(ExpressionPtr left, ExpressionPtr right, MultiplicativeOperator op)
            : left_(std::move(left)), right_(std::move(right)), op_(op) {
            std::cout << "MultiplicativeExpression constructor called" << std::endl;
        };
        // Overload for unary promotion
        MultiplicativeExpression(ExpressionPtr left) : left_(std::move(left)), right_(nullptr), op_(MultiplicativeOperator::UnaryPromote) {};
        void EmitRISC(std::ostream &stream, Context &context, int destReg) const override;
        void Print(std::ostream &stream) const override;
    private:
        ExpressionPtr left_;
        ExpressionPtr right_;
        MultiplicativeOperator op_;
    };

} // namespace ast
