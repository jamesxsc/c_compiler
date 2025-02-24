#pragma once

#include "ast_expression_base.hpp"
#include "ast_and_expression.hpp"

namespace ast {

    class ExclusiveOrExpression;
    using ExclusiveOrExpressionPtr = std::unique_ptr<const ExclusiveOrExpression>;
    class ExclusiveOrExpression : public ExpressionBase {
    public:
        ExclusiveOrExpression(ExclusiveOrExpressionPtr left, AndExpressionPtr right) : left_(std::move(left)), right_(std::move(right)) {}
        // Overload for and promotion
        explicit ExclusiveOrExpression(AndExpressionPtr right) : left_(nullptr), right_(std::move(right)) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;
    private:
        ExclusiveOrExpressionPtr left_;
        AndExpressionPtr right_;
    };

}
