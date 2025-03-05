#pragma once

#include "ast_node.hpp"
#include "ast_type_specifier.hpp"
#include "ast_expression.hpp"

namespace ast {

class ReturnStatement : public Node
{
private:
    ExpressionPtr expression_;

public:
    explicit ReturnStatement(ExpressionPtr expression) : expression_(std::move(expression)) {}

    void EmitRISC(std::ostream& stream, Context& context, Register destReg) const override;
    void Print(std::ostream& stream) const override;

    TypeSpecifier GetType(Context &context) const;
};

} // namespace ast
