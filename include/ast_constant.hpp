#pragma once

#include "ast_node.hpp"
#include "ast_primary_expression.hpp"
#include "ast_type_specifier.hpp"  // So we can return a `Type`

namespace ast {

class IntConstant : public PrimaryExpression
{
private:
    int value_;

public:
    IntConstant(int value) : value_(value) {}

    void EmitRISC(std::ostream& stream, Context& context, Register destReg) const override;
    void Print(std::ostream& stream) const override;

    // Add this override to fix the abstract-class issue
    Type GetType(Context &context) const override
    {
        // For now, just always treat integer constants as `signed int`
        return Type(TypeSpecifier::INT, /*isSigned=*/true);
    }
};

} // namespace ast
