#pragma once

#include "ast_expression_base.hpp"
#include "ast_logical_or_expression.hpp"

namespace ast {

    class Expression; // Forward declaration required to avoid circular dependency
    using ExpressionPtr = std::unique_ptr<const Expression>;

    class ConditionalExpression; // Forward declaration for recursive using declaration
    using ConditionalExpressionPtr = std::unique_ptr<const ConditionalExpression>;
    class ConditionalExpression : public ExpressionBase {
    public:
        ConditionalExpression(LogicalOrExpressionPtr left, ExpressionPtr middle, ConditionalExpressionPtr right);
        // Overload for logical or promotion
        explicit ConditionalExpression(LogicalOrExpressionPtr left);

        void EmitRISC(std::ostream &stream, Context &context, int destReg) const override;
        void Print(std::ostream &stream) const override;
    private:
        bool ternary_;
        LogicalOrExpressionPtr left_;
        ExpressionPtr middle_; // TODO rename existing expression class to expressionbase or equivalent
        ConditionalExpressionPtr right_;
    };

}
