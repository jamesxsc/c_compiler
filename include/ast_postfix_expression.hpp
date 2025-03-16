#pragma once

#include "ast_expression_base.hpp"
#include "ast_expression.hpp"

namespace ast {

    enum class PostfixOperator {
        PrimaryPromote,
        ArrayIndexPromote,
        FunctionCallPromote,
//        MemberAccess,
//        PointerMemberAccess,
        PostfixIncrement,
        PostfixDecrement
    };

    class PostfixExpression : public ExpressionBase {
    public:
        explicit PostfixExpression(ExpressionBasePtr child, PostfixOperator op) : child_(std::move(child)), op_(op) {};

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] double EvaluateFloat(Context &context) const override;

        [[nodiscard]] int Evaluate(Context &context) const override;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        // This should only be called in a context where we know it is an lvalue (identifier)
        [[nodiscard]] std::string GetIdentifier() const;

        [[nodiscard]] const Expression& GetArrayIndexExpression() const;

        TypeSpecifier GetType(Context &context) const override;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;

    private:
        ExpressionBasePtr child_;
        PostfixOperator op_;
    };

    using PostfixExpressionPtr = std::unique_ptr<const PostfixExpression>;

} // namespace ast
