#pragma once

#include "ast_statement.hpp"
#include "ast_expression.hpp"
#include "ast_expression_statement.hpp"

namespace ast {

//--------------------------------------
// WhileStatement
//--------------------------------------
class WhileStatement : public Statement
{
public:
    WhileStatement(ExpressionPtr condition, StatementPtr body);

    void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
    void Print(std::ostream &stream) const override;

private:
    ExpressionPtr condition_;
    StatementPtr  body_;
};

//--------------------------------------
// DoWhileStatement
//--------------------------------------
class DoWhileStatement : public Statement
{
public:
    DoWhileStatement(StatementPtr body, ExpressionPtr condition);

    void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
    void Print(std::ostream &stream) const override;

private:
    StatementPtr  body_;
    ExpressionPtr condition_;
};

//--------------------------------------
// ForStatement
//--------------------------------------
class ForStatement : public Statement
{
public:
    ForStatement(ExpressionStatementPtr initStmt,
                 ExpressionStatementPtr condStmt,
                 ExpressionPtr increment,
                 StatementPtr body);

    void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
    void Print(std::ostream &stream) const override;

private:
    ExpressionStatementPtr initStmt_;
    ExpressionStatementPtr condStmt_;
    ExpressionPtr          increment_;
    StatementPtr           body_;
};

} // namespace ast
