#pragma once

#include "ast_expression_base.hpp"
#include "ast_postfix_expression.hpp"
#include "ast_expression.hpp"

namespace ast {

    class ArrayIndexExpression : public ExpressionBase {
    public:
        ArrayIndexExpression(PostfixExpressionPtr array, ExpressionPtr index) : index_(std::move(index)),
                                                                                array_(std::move(array)) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] int Evaluate(Context &context) const override;
        [[nodiscard]] double EvaluateFloat(Context &context) const override;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        TypeSpecifier GetType(Context &context) const override;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;

        [[nodiscard]] std::string GetIdentifier() const;

        [[nodiscard]] const Expression &GetIndexExpression() const;

    private:
        ExpressionPtr index_;
        PostfixExpressionPtr array_;

        static void EmitLoadElement(const TypeSpecifier &elementType, std::ostream &stream, Context &context,
                             const Register &destReg,
                             const Register &addressTemp) ;
    };

    using ArrayIndexExpressionPtr = std::unique_ptr<const ArrayIndexExpression>;

}
