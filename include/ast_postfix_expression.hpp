#pragma once

#include "ast_expression.hpp"
#include "ast_primary_expression.hpp"

namespace ast {

    class PostfixExpression : public Expression
    {
    public:
        PostfixExpression(ExpressionPtr child) : child_(std::move(child)) {
            std::cout << "PostfixExpression constructor called" << std::endl;
        };
        // TODO support other constructors

        void EmitRISC(std::ostream &stream, Context &context, int destReg) const override;
        void Print(std::ostream &stream) const override;
    private:
        ExpressionPtr child_;
    };

    using PostfixExpressionPtr = std::unique_ptr<const PostfixExpression>;

} // namespace ast
