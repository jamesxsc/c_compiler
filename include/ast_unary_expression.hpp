#pragma once

#include "ast_expression_base.hpp"
#include "ast_postfix_expression.hpp"
#include "ast_type_name.hpp"

namespace ast {

    enum class UnaryOperator {
        PostfixPromote,
        PrefixIncrement,
        PrefixDecrement,
        SizeofUnary,
        SizeofType,
        AddressOf,
        Dereference,
        Plus,
        Minus,
        BitwiseNot,
        LogicalNot
    };

    class MultiplicativeExpression;
    using MultiplicativeExpressionPtr = std::unique_ptr<const MultiplicativeExpression>;

    class UnaryExpression; // Forward declaration for recursive using declaration
    using UnaryExpressionPtr = std::unique_ptr<const UnaryExpression>;

    class UnaryExpression : public ExpressionBase {
    public:
        explicit UnaryExpression(PostfixExpressionPtr child);

        explicit UnaryExpression(TypeNamePtr child);

        UnaryExpression(UnaryExpressionPtr child, UnaryOperator op);

        UnaryExpression(MultiplicativeExpressionPtr child, UnaryOperator op);

        ~UnaryExpression() override;

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void EmitDereferencedAddressRISC(std::ostream &stream, Context &context, Register destReg) const;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] int Evaluate(Context &context) const override;

        [[nodiscard]] double EvaluateFloat(Context &context) const override;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        // This should only be called in a context where we know it is an lvalue (identifier)
        [[nodiscard]] std::string GetIdentifier() const;

        TypeSpecifier GetType(Context &context) const override;

        [[nodiscard]] bool IsPointerDereference() const;

        [[nodiscard]] const Expression &GetArrayIndexExpression() const;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;

    private:
        // Only one of these will ever be non-null depending on the operator (see parser)
        PostfixExpressionPtr postfixChild_ = nullptr;
        UnaryExpressionPtr unaryChild_ = nullptr;
        MultiplicativeExpressionPtr multiplicativeChild_ = nullptr;
        TypeNamePtr typeNameChild_ = nullptr;
        UnaryOperator op_;
    };
}
