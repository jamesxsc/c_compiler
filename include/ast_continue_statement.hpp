#pragma once
#include "ast_statement.hpp"

namespace ast {

class ContinueStatement : public Statement
{
public:
    ContinueStatement() {}

    void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override
    {
        // Minimal stub
        stream << "# continue statement not fully implemented\n";
    }

    void Print(std::ostream &stream) const override
    {
        stream << "continue;";
    }
};

} // namespace ast
