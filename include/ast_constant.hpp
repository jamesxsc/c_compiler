#pragma once

#include "ast_node.hpp"
#include "ast_primary_expression.hpp"

namespace ast {
// TODO decide if this stays in here/ what gets shifted into PrimaryExpression
class IntConstant : public PrimaryExpression
{
private:
    int value_;

public:
    IntConstant(int value) : value_(value) {}

    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;
};

} // namespace ast
