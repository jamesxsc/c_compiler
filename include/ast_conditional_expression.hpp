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
        TypeSpecifier GetType(Context &context) const override;
        ConditionalExpression(LogicalOrExpressionPtr left, ExpressionPtr middle, ConditionalExpressionPtr right);
        // Overload for logical or promotion
        explicit ConditionalExpression(LogicalOrExpressionPtr left);

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;
        [[nodiscard]] int Evaluate(Context &context) const override;
        [[nodiscard]] bool ContainsFunctionCall() const override;
        [[nodiscard]] std::string GetGlobalIdentifier() const override;

    private:
        bool ternary_;
        LogicalOrExpressionPtr left_;
        ExpressionPtr middle_;
        ConditionalExpressionPtr right_;
    };

}
