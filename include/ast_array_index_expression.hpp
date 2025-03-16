#pragma once

#include "ast_expression_base.hpp"
#include "ast_postfix_expression.hpp"
#include "ast_expression.hpp"

// If Register or Context aren't recognized, also include their headers:
// #include "register.hpp"
// #include "context.hpp"

namespace ast {

    class ArrayIndexExpression : public ExpressionBase {
    public:
        ArrayIndexExpression(PostfixExpressionPtr array, ExpressionPtr index)
            : index_(std::move(index)), array_(std::move(array)) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        Register EmitAddressRISC(std::ostream &stream, Context &context) const;

        void Print(std::ostream &stream) const override;

        int Evaluate() const override;

        bool ContainsFunctionCall() const override;

        TypeSpecifier GetType(Context &context) const override;

        std::string GetGlobalIdentifier() const override;

        int GetGlobalValue() const override;

        std::string GetIdentifier() const;

        const Expression &GetIndexExpression() const;

    private:
        ExpressionPtr index_;
        PostfixExpressionPtr array_;
    };

    using ArrayIndexExpressionPtr = std::unique_ptr<const ArrayIndexExpression>;

} // namespace ast
