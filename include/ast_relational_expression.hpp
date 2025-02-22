#pragma once

#include "ast_expression.hpp"
#include "ast_shift_expression.hpp"

namespace ast {

    enum class RelationalOperator {
        ShiftPromote,
        LessThan,
        GreaterThan,
        LessThanOrEqual,
        GreaterThanOrEqual
    };

    class RelationalExpression;

    using RelationalExpressionPtr = std::shared_ptr<const RelationalExpression>;

    class RelationalExpression : public Expression {
    public:
        RelationalExpression(RelationalExpressionPtr left, ShiftExpressionPtr right, RelationalOperator op) : left_(
                std::move(left)), right_(std::move(right)), op_(op) {}

        // Overload for shift promote
        explicit RelationalExpression(ShiftExpressionPtr right) : left_(nullptr), right_(std::move(right)),
                                                                  op_(RelationalOperator::ShiftPromote) {}

        void EmitRISC(std::ostream &stream, Context &context, int destReg) const override;

        void Print(std::ostream &stream) const override;

    private:
        RelationalExpressionPtr left_;
        ShiftExpressionPtr right_;
        RelationalOperator op_;
    };

}
