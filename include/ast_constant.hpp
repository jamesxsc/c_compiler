#pragma once

#include "ast_node.hpp"
#include "ast_primary_expression.hpp"
#include "ast_type_specifier.hpp"
#include "ast_expression_base.hpp"

namespace ast {

class IntConstant : public PrimaryExpression
{
private:
    int value_;

public:
    IntConstant(int value) : value_(value) {}

    void EmitRISC(std::ostream& stream, Context& context, Register destReg) const override;
    void Print(std::ostream& stream) const override;

    Type GetType(Context &context) const override
    {
        return Type(TypeSpecifier::INT, true);
    }
};

class FloatConstant : public PrimaryExpression
{
private:
    double value_;

public:
    FloatConstant(double value) : value_(value) {}

    void EmitRISC(std::ostream& stream, Context& context, Register destReg) const override;
    void Print(std::ostream& stream) const override;

    Type GetType(Context &context) const override
    {
        return Type(TypeSpecifier::FLOAT, true);
    }
};

} // namespace ast
