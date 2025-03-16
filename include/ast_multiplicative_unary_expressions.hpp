#pragma once

#include "ast_expression_base.hpp"
#include "ast_postfix_expression.hpp"
#include "ast_type_name.hpp"
#include "ast_expression.hpp"

namespace ast {

    enum class MultiplicativeOperator {
        UnaryPromote,
        Multiply,
        Divide,
        Modulo
    };

    class MultiplicativeExpression;
    using MultiplicativeExpressionPtr = std::unique_ptr<const MultiplicativeExpression>;

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

    class UnaryExpression;
    using UnaryExpressionPtr = std::unique_ptr<const UnaryExpression>;

    class UnaryExpression : public ExpressionBase {
    public:
        explicit UnaryExpression(PostfixExpressionPtr child)
            : postfixChild_(std::move(child)), op_(UnaryOperator::PostfixPromote) {}

        explicit UnaryExpression(TypeNamePtr child)
            : typeNameChild_(std::move(child)), op_(UnaryOperator::SizeofType) {}

        UnaryExpression(UnaryExpressionPtr child, UnaryOperator op)
            : unaryChild_(std::move(child)), op_(op) {}

        UnaryExpression(MultiplicativeExpressionPtr child, UnaryOperator op)
            : multiplicativeChild_(std::move(child)), op_(op) {}

        ~UnaryExpression();

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;
<<<<<<< HEAD
        int Evaluate() const override;
        bool ContainsFunctionCall() const override;
        std::string GetIdentifier() const;
        TypeSpecifier GetType(Context &context) const override;
        bool IsPointerDereference() const override { return op_ == UnaryOperator::Dereference; }
        const Expression &GetArrayIndexExpression() const;
        int GetGlobalValue() const override;
        std::string GetGlobalIdentifier() const override;
=======

        [[nodiscard]] int Evaluate(Context &context) const override;

        [[nodiscard]] double EvaluateFloat(Context &context) const override;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        // This should only be called in a context where we know it is an lvalue (identifier)
        [[nodiscard]] std::string GetIdentifier() const;

        TypeSpecifier GetType(Context &context) const override;

        [[nodiscard]] bool IsPointerDereference() const;

        [[nodiscard]] const Expression &GetArrayIndexExpression() const;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;
>>>>>>> aaabd7d9f74208ee579772452eb7f2b51bfa0e1d

    private:
        PostfixExpressionPtr postfixChild_ = nullptr;
        UnaryExpressionPtr unaryChild_ = nullptr;
        MultiplicativeExpressionPtr multiplicativeChild_ = nullptr;
        TypeNamePtr typeNameChild_ = nullptr;
        UnaryOperator op_;
    };

    class MultiplicativeExpression : public ExpressionBase {
    public:
        TypeSpecifier GetType(Context &context) const override;

        MultiplicativeExpression(MultiplicativeExpressionPtr left, UnaryExpressionPtr right, MultiplicativeOperator op)
            : left_(std::move(left)), right_(std::move(right)), op_(op) {}

        explicit MultiplicativeExpression(UnaryExpressionPtr right)
            : left_(nullptr), right_(std::move(right)), op_(MultiplicativeOperator::UnaryPromote) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;
<<<<<<< HEAD
        int Evaluate() const override;
        std::string GetIdentifier() const;
        bool ContainsFunctionCall() const override;
        int GetGlobalValue() const override;
        std::string GetGlobalIdentifier() const override;
=======

        [[nodiscard]] int Evaluate(Context &context) const override;

        [[nodiscard]] double EvaluateFloat(Context &context) const override;

        [[nodiscard]] std::string GetIdentifier() const;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;
>>>>>>> aaabd7d9f74208ee579772452eb7f2b51bfa0e1d

    private:
        MultiplicativeExpressionPtr left_;
        UnaryExpressionPtr right_;
        MultiplicativeOperator op_;
    };

} 
