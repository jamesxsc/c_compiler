#pragma once

#include "ast_expression_base.hpp"
#include "ast_postfix_expression.hpp"

// TODO if we can split into separate files, but its a pain because of the circular includes
namespace ast {

    enum class MultiplicativeOperator {
        UnaryPromote,
        Multiply,
        Divide,
        Modulo
    };

    class MultiplicativeExpression; // Forward declaration for recursive using declaration
    using MultiplicativeExpressionPtr = std::unique_ptr<const MultiplicativeExpression>;

    enum class UnaryOperator {
        PostfixPromote,
        PrefixIncrement,
        PrefixDecrement,
//        SizeofUnary,
//        SizeofType, // todo impl sizeof

        AddressOf,
        Dereference,
        Plus,
        Minus,
        BitwiseNot,
        LogicalNot
    };

    class UnaryExpression; // Forward declaration for recursive using declaration
    using UnaryExpressionPtr = std::unique_ptr<const UnaryExpression>;
    class UnaryExpression : public ExpressionBase
    {
    public:
        explicit UnaryExpression(PostfixExpressionPtr child) : postfixChild_(std::move(child)), op_(UnaryOperator::PostfixPromote) {};
        UnaryExpression(UnaryExpressionPtr child, UnaryOperator op) : unaryChild_(std::move(child)), op_(op) {};
        UnaryExpression(MultiplicativeExpressionPtr child, UnaryOperator op) : multiplicativeChild_(std::move(child)), op_(op) {};
        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;
        [[nodiscard]] bool ContainsFunctionCall() const override;
        // This should only be called in a context where we know it is an lvalue (identifier)
        [[nodiscard]] std::string GetIdentifier() const;
        Type GetType(Context &context) const override;

    private:
        // Only one of these will ever be non-null depending on the operator (see parser)
        PostfixExpressionPtr postfixChild_ = nullptr;
        UnaryExpressionPtr unaryChild_ = nullptr;
        MultiplicativeExpressionPtr multiplicativeChild_ = nullptr;
        UnaryOperator op_;
    };

    class MultiplicativeExpression : public ExpressionBase
    {
    public:
        Type GetType(Context &context) const override;
        MultiplicativeExpression(MultiplicativeExpressionPtr left, UnaryExpressionPtr right, MultiplicativeOperator op)
            : left_(std::move(left)), right_(std::move(right)), op_(op) {}
        // Overload for unary promotion
        explicit MultiplicativeExpression(UnaryExpressionPtr right) : left_(nullptr), right_(std::move(right)), op_(MultiplicativeOperator::UnaryPromote) {}
        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;
        [[nodiscard]] std::string GetIdentifier() const;
        [[nodiscard]] bool ContainsFunctionCall() const override;
    private:
        MultiplicativeExpressionPtr left_;
        UnaryExpressionPtr right_;
        MultiplicativeOperator op_;
    };

} // namespace ast
