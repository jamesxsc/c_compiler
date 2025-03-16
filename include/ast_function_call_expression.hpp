#pragma once

#include "ast_expression_base.hpp"
#include "ast_argument_expression_list.hpp"
#include "ast_postfix_expression.hpp"

namespace ast {

    class FunctionCallExpression : public ExpressionBase {
    public:
        FunctionCallExpression(PostfixExpressionPtr function, ArgumentExpressionListPtr arguments)
                : function_(std::move(function)), arguments_(std::move(arguments)) {}

        explicit FunctionCallExpression(PostfixExpressionPtr function)
                : function_(std::move(function)), arguments_(nullptr) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] double EvaluateFloat(Context &context) const override;
        [[nodiscard]] int Evaluate(Context &context) const override;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        TypeSpecifier GetType(Context &context) const override;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;

    private:
        PostfixExpressionPtr function_;
        ArgumentExpressionListPtr arguments_; // Can be null
    };

}
