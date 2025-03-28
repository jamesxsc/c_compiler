#pragma once

#include "ast_expression_base.hpp"
#include "ast_additive_expression.hpp"

namespace ast {

    enum class ShiftOperator {
        AdditivePromote,
        Left,
        Right
    };

    class ShiftExpression; // Forward declaration for recursive using declaration
    using ShiftExpressionPtr = std::unique_ptr<const ShiftExpression>;
    class ShiftExpression : public ExpressionBase {
    public:
        TypeSpecifier GetType(Context &context) const override;
        ShiftExpression(ShiftExpressionPtr left, AdditiveExpressionPtr right, ShiftOperator op) : left_(std::move(left)), right_(std::move(right)), op_(op) {}
        // Overload for additive promotion
        explicit ShiftExpression(AdditiveExpressionPtr right) : left_(nullptr), right_(std::move(right)), op_(ShiftOperator::AdditivePromote) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] double EvaluateFloat(Context &context) const override;
        [[nodiscard]] int Evaluate(Context &context) const override;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;

    private:
        ShiftExpressionPtr left_;
        AdditiveExpressionPtr right_;
        ShiftOperator op_;
    };

}
