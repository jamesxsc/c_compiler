#pragma once

#include "ast_expression_base.hpp"

namespace ast {

    class ConditionalExpression;

    class UnaryExpression;

    using ConditionalExpressionPtr = std::unique_ptr<const ConditionalExpression>;
    using UnaryExpressionPtr = std::unique_ptr<const UnaryExpression>;

    enum class AssignmentOperator {
        ConditionalPromote,
        Assign,
        MultiplyAssign,
        DivideAssign,
        ModuloAssign,
        AddAssign,
        SubtractAssign,
        LeftShiftAssign,
        RightShiftAssign,
        BitwiseAndAssign,
        BitwiseXorAssign,
        BitwiseOrAssign
    };

    class Expression; // Forward declaration for recursive using declaration
    using ExpressionPtr = std::unique_ptr<const Expression>;

    class AssignmentExpression; // Forward declaration for recursive using declaration
    using AssignmentExpressionPtr = std::unique_ptr<const AssignmentExpression>;

    class AssignmentExpression : public ExpressionBase {
    public:
        ~AssignmentExpression() override;

        TypeSpecifier GetType(Context &context) const override;

        explicit AssignmentExpression(ConditionalExpressionPtr conditional);

        AssignmentExpression(UnaryExpressionPtr unary, AssignmentOperator op, AssignmentExpressionPtr assignment);

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] double EvaluateFloat(Context &context) const override;
        [[nodiscard]] int Evaluate(Context &context) const override;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;

    private:
        AssignmentOperator op_;

        // For promotion
        ConditionalExpressionPtr conditional_;
        // For assignment
        UnaryExpressionPtr unary_;
        AssignmentExpressionPtr assignment_;

        TypeSpecifier GetTypeInternal(Context &context) const;
    };


}
