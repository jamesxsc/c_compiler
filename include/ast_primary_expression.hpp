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
    };

} // namespace ast
