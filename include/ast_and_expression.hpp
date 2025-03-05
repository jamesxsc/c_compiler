#pragma once

#include <utility>

#include "ast_expression_base.hpp"
#include "ast_equality_expression.hpp"

// Note: This refers to the bitwise & operator precedence level
namespace ast {

    class AndExpression;

    using AndExpressionPtr = std::unique_ptr<const AndExpression>;

    class AndExpression : public ExpressionBase {
    public:
        TypeSpecifier GetType(Context &context) const override;
        AndExpression(AndExpressionPtr left, EqualityExpressionPtr right) : left_(std::move(left)), right_(std::move(right)) {}
        // Overload for equality promotion
        explicit AndExpression(EqualityExpressionPtr right) : left_(nullptr), right_(std::move(right)) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] bool ContainsFunctionCall() const override;

    private:
        AndExpressionPtr left_;
        EqualityExpressionPtr right_;
    };

}
