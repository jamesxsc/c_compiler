#pragma once

#include "ast_expression_base.hpp"
#include "ast_exclusive_or_expression.hpp"

namespace ast {

    class InclusiveOrExpression;
    using InclusiveOrExpressionPtr = std::unique_ptr<const InclusiveOrExpression>;
    class InclusiveOrExpression : public ExpressionBase {
    public:
        TypeSpecifier GetType(Context &context) const override;
        InclusiveOrExpression(InclusiveOrExpressionPtr left, ExclusiveOrExpressionPtr right) : left_(std::move(left)), right_(std::move(right)) {}
        // Overload for exclusive or promotion
        explicit InclusiveOrExpression(ExclusiveOrExpressionPtr right) : left_(nullptr), right_(std::move(right)) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;
        [[nodiscard]] double EvaluateFloat(Context &context) const override;
        [[nodiscard]] int Evaluate(Context &context) const override;
        [[nodiscard]] bool ContainsFunctionCall() const override;
        [[nodiscard]] std::string GetGlobalIdentifier() const override;

    private:
        InclusiveOrExpressionPtr left_;
        ExclusiveOrExpressionPtr right_;
    };

}
