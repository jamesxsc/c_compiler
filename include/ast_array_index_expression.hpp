#pragma once

#include "ast_expression_base.hpp"
#include "ast_postfix_expression.hpp"
#include "ast_expression.hpp"

namespace ast {

    class ArrayIndexExpression : public ExpressionBase {
    public:
        ArrayIndexExpression(PostfixExpressionPtr array, ExpressionPtr index) : array_(std::move(array)), index_(std::move(index)) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;
        [[nodiscard]] bool ContainsFunctionCall() const override;
        TypeSpecifier GetType(Context &context) const override;
        [[nodiscard]] std::string GetGlobalIdentifier() const override;
        [[nodiscard]] int GetGlobalValue() const override;
    private:
        PostfixExpressionPtr array_;
        ExpressionPtr index_;
    };

}
