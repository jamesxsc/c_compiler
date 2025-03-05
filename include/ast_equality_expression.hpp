#pragma once

#include "ast_expression_base.hpp"
#include "ast_relational_expression.hpp"

namespace ast {

    enum class EqualityOperator {
        RelationalPromote,
        Equality,
        Inequality
    };

    class EqualityExpression; 
    using EqualityExpressionPtr = std::unique_ptr<const EqualityExpression>;

    class EqualityExpression : public ExpressionBase {
    public:
        Type GetType(Context &context) const override;

        EqualityExpression(EqualityExpressionPtr left, RelationalExpressionPtr right, EqualityOperator op)
            : left_(std::move(left)), right_(std::move(right)), op_(op) {}


        explicit EqualityExpression(RelationalExpressionPtr right)
            : left_(nullptr), right_(std::move(right)), op_(EqualityOperator::RelationalPromote) {}

        ~EqualityExpression(); 

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;

    private:
        EqualityExpressionPtr left_;
        RelationalExpressionPtr right_;
        EqualityOperator op_;
    };

} // namespace ast
