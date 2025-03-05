#pragma once

#include "ast_expression_base.hpp"
#include "ast_conditional_expression.hpp"
#include "ast_unary_expression.hpp"

namespace ast {

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

    class Expression; 
    using ExpressionPtr = std::unique_ptr<const Expression>;

    class AssignmentExpression;
    using AssignmentExpressionPtr = std::unique_ptr<const AssignmentExpression>;

    class AssignmentExpression : public ExpressionBase {
    public:
        Type GetType(Context &context) const override;
        explicit AssignmentExpression(ConditionalExpressionPtr conditional);
        AssignmentExpression(UnaryExpressionPtr unary, AssignmentOperator op, AssignmentExpressionPtr assignment);

        ~AssignmentExpression();  

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;

    private:
        AssignmentOperator op_;
        ConditionalExpressionPtr conditional_;
        UnaryExpressionPtr unary_;
        AssignmentExpressionPtr assignment_;
    };

}
