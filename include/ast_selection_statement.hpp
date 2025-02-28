#pragma once

#include "ast_statement.hpp"
#include "ast_expression.hpp"

namespace ast {

//--------------------------------------
// IfStatement
//--------------------------------------
class IfStatement : public Statement
{
public:
    IfStatement(ExpressionPtr condition, StatementPtr thenStmt, StatementPtr elseStmt = nullptr);

    // Use a named destReg parameter
    void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
    void Print(std::ostream &stream) const override;

private:
    ExpressionPtr condition_;
    StatementPtr  thenStmt_;
    StatementPtr  elseStmt_;
};

//--------------------------------------
// SwitchStatement
//--------------------------------------
class SwitchStatement : public Statement
{
public:
    SwitchStatement(ExpressionPtr condition, StatementPtr body);

    void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
    void Print(std::ostream &stream) const override;

private:
    ExpressionPtr condition_;
    StatementPtr  body_;
};

} // namespace ast
