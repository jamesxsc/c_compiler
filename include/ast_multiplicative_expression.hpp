#pragma once

#include "ast_expression_base.hpp"
#include "ast_postfix_expression.hpp"
#include "ast_type_name.hpp"
#include "ast_expression.hpp"

namespace ast {

    enum class MultiplicativeOperator {
        UnaryPromote,
        Multiply,
        Divide,
        Modulo
    };

    class UnaryExpression;
    using UnaryExpressionPtr = std::unique_ptr<const UnaryExpression>;

    class MultiplicativeExpression; // Forward declaration for recursive using declaration
    using MultiplicativeExpressionPtr = std::unique_ptr<const MultiplicativeExpression>;

    class MultiplicativeExpression : public ExpressionBase {
    public:
        TypeSpecifier GetType(Context &context) const override;

        ~MultiplicativeExpression() override;

        MultiplicativeExpression(MultiplicativeExpressionPtr left, UnaryExpressionPtr right, MultiplicativeOperator op);

        // Overload for unary promotion
        explicit MultiplicativeExpression(UnaryExpressionPtr right);

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] int Evaluate(Context &context) const override;

        [[nodiscard]] double EvaluateFloat(Context &context) const override;

        [[nodiscard]] std::string GetIdentifier() const;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;

    private:
        MultiplicativeExpressionPtr left_;
        UnaryExpressionPtr right_;
        MultiplicativeOperator op_;
    };

} // namespace ast
