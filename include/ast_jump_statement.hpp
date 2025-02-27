#pragma once

#include "ast_node.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

class ReturnStatement : public Node
{
private:
    NodePtr expression_;

public:
    explicit ReturnStatement(NodePtr expression) : expression_(std::move(expression)) {}

    void EmitRISC(std::ostream& stream, Context& context, Register destReg) const override;
    void Print(std::ostream& stream) const override;

    Type GetType(Context &context) const; 
};

} // namespace ast
