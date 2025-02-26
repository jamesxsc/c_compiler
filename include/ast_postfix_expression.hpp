#pragma once

#include "ast_expression_base.hpp"
#include "ast_primary_expression.hpp"

namespace ast {

    class PostfixExpression : public ExpressionBase {
    public:
        explicit PostfixExpression(ExpressionBasePtr child) : child_(std::move(child)) {
            std::cout << "PostfixExpression constructor called" << std::endl;
        };

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        // This should only be called in a context where we know it is an lvalue (identifier)
        [[nodiscard]] std::string GetIdentifier() const;

    private:
        ExpressionBasePtr child_;
    };

    using PostfixExpressionPtr = std::unique_ptr<const PostfixExpression>;

} // namespace ast
