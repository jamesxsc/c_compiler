#pragma once

#include "ast_expression.hpp"

namespace ast {

    class PrimaryExpression : public Expression
    {
    // Abstract for now
    public:
        virtual ~PrimaryExpression() = default;
        virtual void EmitRISC(std::ostream &stream, Context &context, int destReg) const override = 0;
        virtual void Print(std::ostream &stream) const override = 0;
    };

} // namespace ast
