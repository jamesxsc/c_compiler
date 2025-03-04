#pragma once
#include "ast_statement.hpp"

namespace ast {

class BreakStatement : public Statement
{
public:
    BreakStatement() {}

    void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override
    {
        stream << "# break statement not fully implemented\n";
    }

    void Print(std::ostream &stream) const override
    {
        stream << "break;";
    }
};

} // namespace ast
