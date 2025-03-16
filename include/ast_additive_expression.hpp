#pragma once

#include "ast_expression_base.hpp"
#include "ast_multiplicative_unary_expressions.hpp"
#include "ast_type_specifier.hpp"
namespace ast {

    enum class AdditiveOperator {
        MultiplicativePromote,
        Add,
        Subtract
    };

    class AdditiveExpression; // Forward declaration for recursive using declaration
    using AdditiveExpressionPtr = std::unique_ptr<const AdditiveExpression>;

    class AdditiveExpression : public ExpressionBase {
    public:
        TypeSpecifier GetType(Context &context) const override;
        AdditiveExpression(AdditiveExpressionPtr left, MultiplicativeExpressionPtr right, AdditiveOperator op) : left_(
                std::move(left)), right_(std::move(right)), op_(op) {}

        // Overload for multiplicative promotion
        explicit AdditiveExpression(MultiplicativeExpressionPtr right) : left_(nullptr), right_(std::move(right)),
                                                                         op_(AdditiveOperator::MultiplicativePromote) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] double EvaluateFloat(Context &context) const override;
        [[nodiscard]] int Evaluate(Context &context) const override;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;

    private:
        AdditiveExpressionPtr left_;
        MultiplicativeExpressionPtr right_;
        AdditiveOperator op_;
    };

}

