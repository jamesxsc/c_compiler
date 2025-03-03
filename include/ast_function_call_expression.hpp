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

        [[nodiscard]] bool ContainsFunctionCall() const override;

        Type GetType(Context &context) const override;

    private:
        PostfixExpressionPtr function_;
        ArgumentExpressionListPtr arguments_; // Can be null
    };

    using FunctionCallExpressionPtr = std::unique_ptr<const FunctionCallExpression>;

}
