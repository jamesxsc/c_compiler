#pragma once

#include "ast_node.hpp"
#include "ast_type_specifier.hpp"
#include "ast_expression.hpp"
#include "ast_statement.hpp"

namespace ast {

class JumpStatement : public Statement
{
public:
    void EmitRISC(std::ostream& stream, Context& context, Register destReg) const override = 0;
    void Print(std::ostream& stream) const override = 0;
};

class ReturnStatement : public JumpStatement
{
private:
    ExpressionPtr expression_;

public:
    explicit ReturnStatement(ExpressionPtr expression) : expression_(std::move(expression)) {}

    void EmitRISC(std::ostream& stream, Context& context, Register destReg) const override;
    void Print(std::ostream& stream) const override;

    TypeSpecifier GetType(Context &context) const;
};

class BreakStatement : public JumpStatement
{
public:
    void EmitRISC(std::ostream& stream, Context& context, Register destReg) const override;
    void Print(std::ostream& stream) const override;
};

class ContinueStatement : public JumpStatement
{
public:
    void EmitRISC(std::ostream& stream, Context& context, Register destReg) const override;
    void Print(std::ostream& stream) const override;
};

} // namespace ast
