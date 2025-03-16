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

<<<<<<< HEAD
        int Evaluate() const override;
=======
        [[nodiscard]] int Evaluate(Context &context) const override;
>>>>>>> aaabd7d9f74208ee579772452eb7f2b51bfa0e1d

        bool ContainsFunctionCall() const override;

        TypeSpecifier GetType(Context &context) const override;

        std::string GetGlobalIdentifier() const override;

<<<<<<< HEAD
        int GetGlobalValue() const override;

        std::string GetIdentifier() const;
=======
        [[nodiscard]] std::string GetIdentifier() const;
>>>>>>> aaabd7d9f74208ee579772452eb7f2b51bfa0e1d

        const Expression &GetIndexExpression() const;

    private:
        ExpressionPtr index_;
        PostfixExpressionPtr array_;

        double EvaluateFloat(Context &context) const;
    };

    using ArrayIndexExpressionPtr = std::unique_ptr<const ArrayIndexExpression>;

} // namespace ast
