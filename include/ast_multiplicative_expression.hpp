#pragma once

#include "ast_expression_base.hpp"
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

    class MultiplicativeExpression : public ExpressionBase
    {
    public:
        Type GetType(Context &context) const override;
        MultiplicativeExpression(MultiplicativeExpressionPtr left, UnaryExpressionPtr right, MultiplicativeOperator op)
            : left_(std::move(left)), right_(std::move(right)), op_(op) {}
        // Overload for unary promotion
        explicit MultiplicativeExpression(UnaryExpressionPtr right) : left_(nullptr), right_(std::move(right)), op_(MultiplicativeOperator::UnaryPromote) {}
        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;
        [[nodiscard]] bool ContainsFunctionCall() const override;
    private:
        MultiplicativeExpressionPtr left_;
        UnaryExpressionPtr right_;
        MultiplicativeOperator op_;
    };

} // namespace ast
