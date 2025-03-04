#pragma once
#include "ast_primary_expression.hpp"

namespace ast {

class StringLiteral : public PrimaryExpression
{
private:
    std::string value_;

public:
    explicit StringLiteral(const std::string &str)
        : value_(str)
    {
    }

    void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override
    {
        stream << "# String literal not fully implemented\n";
    }

    void Print(std::ostream &stream) const override
    {
        stream << "\"" << value_ << "\"";
    }

    Type GetType(Context&) const override
    {
        return Type(TypeSpecifier::INT, true);
    }
};

} // namespace ast
