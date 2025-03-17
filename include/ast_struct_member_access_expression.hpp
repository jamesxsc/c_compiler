#pragma once

#include "ast_expression_base.hpp"
#include "ast_postfix_expression.hpp"

namespace ast {

    class StructMemberAccessExpression : public ExpressionBase {
    public:
        StructMemberAccessExpression(PostfixExpressionPtr s, std::string member, bool pointerAccess)
                : struct_(std::move(s)), member_(std::move(member)), pointerAccess_(pointerAccess) {}

        StructMemberAccessExpression(PostfixExpressionPtr s, std::string member)
                : struct_(std::move(s)), member_(std::move(member)), pointerAccess_(false) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        TypeSpecifier GetType(Context &context) const override;

        [[nodiscard]] double EvaluateFloat(Context &context) const override;

        [[nodiscard]] int Evaluate(Context &context) const override;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;

    private:
        PostfixExpressionPtr struct_;
        std::string member_;
        bool pointerAccess_;
    };

}
