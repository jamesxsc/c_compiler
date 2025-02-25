#pragma once

#include "ast_expression_base.hpp"

namespace ast {

    class PostfixExpression : public ExpressionBase {
    private:
        ExpressionBasePtr child_;

    public:
        explicit PostfixExpression(ExpressionBasePtr child) : child_(std::move(child)) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;
        Type GetType(Context &context) const override; 
    };

    using PostfixExpressionPtr = std::unique_ptr<const PostfixExpression>;

}
