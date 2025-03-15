#pragma once

#include "ast_expression_base.hpp"
#include "ast_inclusive_or_expression.hpp"

namespace ast {

    class LogicalAndExpression; // Forward declaration for recursive using declaration
    using LogicalAndExpressionPtr = std::unique_ptr<const LogicalAndExpression>;

    class LogicalAndExpression : public ExpressionBase {
    public:
        TypeSpecifier GetType(Context &context) const override;
        LogicalAndExpression(LogicalAndExpressionPtr left, InclusiveOrExpressionPtr right) : left_(std::move(left)), right_(std::move(right)) {}
        // Overload for inclusive or promotion
        explicit LogicalAndExpression(InclusiveOrExpressionPtr right) : left_(nullptr), right_(std::move(right)) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;
        [[nodiscard]] int Evaluate() const override;
        [[nodiscard]] bool ContainsFunctionCall() const override;
        [[nodiscard]] std::string GetGlobalIdentifier() const override;
        [[nodiscard]] int GetGlobalValue() const override;
    private:
        LogicalAndExpressionPtr left_;
        InclusiveOrExpressionPtr right_;
    };

}
