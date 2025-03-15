#pragma once

#include "ast_assignment_expression.hpp"
#include "ast_expression_base.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    class Expression; // Forward declaration
    using ExpressionPtr = std::unique_ptr<const Expression>;
    using AssignmentExpressionPtr = std::unique_ptr<const AssignmentExpression>;

    class Expression : public ExpressionBase {
    public:
        Expression(ExpressionPtr first, AssignmentExpressionPtr assignment)
            : assignment_(std::move(assignment))
            , first_(std::move(first))
        {}

        explicit Expression(AssignmentExpressionPtr assignment)
            : assignment_(std::move(assignment))
            , first_(nullptr)
        {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] int Evaluate() const override;
        [[nodiscard]] bool ContainsFunctionCall() const override;
        TypeSpecifier GetType(Context &context) const override;
        [[nodiscard]] std::string GetGlobalIdentifier() const override;
        [[nodiscard]] int GetGlobalValue() const override;

    private:
        AssignmentExpressionPtr assignment_;
        ExpressionPtr first_;
    };

} // namespace ast
