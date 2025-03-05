#pragma once

#include "ast_expression_base.hpp"
#include "ast_inclusive_or_expression.hpp"

namespace ast {

    class LogicalAndExpression; 
    using LogicalAndExpressionPtr = std::unique_ptr<const LogicalAndExpression>;

    class LogicalAndExpression : public ExpressionBase {
    public:
        Type GetType(Context &context) const override;
        LogicalAndExpression(LogicalAndExpressionPtr left, InclusiveOrExpressionPtr right) : left_(std::move(left)), right_(std::move(right)) {}
        explicit LogicalAndExpression(InclusiveOrExpressionPtr right) : left_(nullptr), right_(std::move(right)) {}
        ~LogicalAndExpression();
        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;
    private:
        LogicalAndExpressionPtr left_;
        InclusiveOrExpressionPtr right_;
    };

}
