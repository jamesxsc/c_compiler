#pragma once

#include "ast_expression.hpp"
#include "ast_unary_expression.hpp"

namespace ast {

    enum class MultiplicativeOperator {
        UnaryPromote,
        Multiply,
        Divide,
        Modulo
    };

    class MultiplicativeExpression; // Forward declaration for recursive using declaration
    using MultiplicativeExpressionPtr = std::unique_ptr<const MultiplicativeExpression>;

    class MultiplicativeExpression : public Expression
    {
    public:
        MultiplicativeExpression(MultiplicativeExpressionPtr left, UnaryExpressionPtr right, MultiplicativeOperator op)
            : left_(std::move(left)), right_(std::move(right)), op_(op) {
            std::cout << "MultiplicativeExpression constructor called" << std::endl;
        };
        // Overload for unary promotion
        MultiplicativeExpression(UnaryExpressionPtr left) : left_(nullptr), right_(std::move(left)), op_(MultiplicativeOperator::UnaryPromote) {};
        void EmitRISC(std::ostream &stream, Context &context, int destReg) const override;
        void Print(std::ostream &stream) const override;
    private:
        ExpressionPtr left_;
        UnaryExpressionPtr right_;
        MultiplicativeOperator op_;
    };

} // namespace ast
