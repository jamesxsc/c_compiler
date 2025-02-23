#pragma once

#include "ast_assignment_expression.hpp"
#include "ast_expression_base.hpp"

// Top level of expression hierarchy
namespace ast {

    class Expression; // Forward declaration for recursive using declaration
    using ExpressionPtr = std::unique_ptr<const Expression>;
    using AssignmentExpressionPtr = std::unique_ptr<const AssignmentExpression>;
    class Expression : public ExpressionBase {
    public:
        Expression(ExpressionPtr first, AssignmentExpressionPtr assignment) : assignment_(std::move(assignment)), first_(std::move(first)) {}
        explicit Expression(AssignmentExpressionPtr assignment) : assignment_(std::move(assignment)), first_(nullptr) {}

        void EmitRISC(std::ostream &stream, Context &context, int destReg) const override;
        void Print(std::ostream &stream) const override;
    private:
        AssignmentExpressionPtr assignment_;
        ExpressionPtr first_;
    };

}
