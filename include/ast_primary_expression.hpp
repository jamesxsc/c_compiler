#pragma once

#include "ast_expression_base.hpp"

namespace ast {

    class PrimaryExpression : public ExpressionBase
    {
    // Abstract for now
    public:
        virtual ~PrimaryExpression() = default;
        virtual void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override = 0;
        virtual void Print(std::ostream &stream) const override = 0;
        [[nodiscard]] virtual int Evaluate() const override = 0;
        virtual TypeSpecifier GetType(Context &context) const override = 0;
        [[nodiscard]] virtual bool ContainsFunctionCall() const override = 0;
    };

} // namespace ast
