#include "ast_iteration_statement.hpp"
#include <iostream>

namespace ast {

//==================== WhileStatement ====================//
WhileStatement::WhileStatement(ExpressionPtr condition, StatementPtr body)
  : condition_(std::move(condition))
  , body_(std::move(body))
{}

void WhileStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const
{
    stream << "# WhileStatement EmitRISC (stub)\n";

    if (condition_) condition_->EmitRISC(stream, context, destReg);
    if (body_)      body_->EmitRISC(stream, context, destReg);
}

void WhileStatement::Print(std::ostream &stream) const
{
    stream << "WhileStatement(\n";
    if (condition_) {
        stream << "  condition: ";
        condition_->Print(stream);
        stream << "\n";
    }
    if (body_) {
        stream << "  body:\n    ";
        body_->Print(stream);
        stream << "\n";
    }
    stream << ")\n";
}

//==================== DoWhileStatement ====================//
DoWhileStatement::DoWhileStatement(StatementPtr body, ExpressionPtr condition)
  : body_(std::move(body))
  , condition_(std::move(condition))
{}

void DoWhileStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const
{
    stream << "# DoWhileStatement EmitRISC (stub)\n";

    if (body_) body_->EmitRISC(stream, context, destReg);
    if (condition_) condition_->EmitRISC(stream, context, destReg);
}

void DoWhileStatement::Print(std::ostream &stream) const
{
    stream << "DoWhileStatement(\n";
    if (body_) {
        stream << "  body:\n    ";
        body_->Print(stream);
        stream << "\n";
    }
    if (condition_) {
        stream << "  condition: ";
        condition_->Print(stream);
        stream << "\n";
    }
    stream << ")\n";
}

//==================== ForStatement ====================//
ForStatement::ForStatement(ExpressionStatementPtr initStmtParam,
                           ExpressionStatementPtr condStmtParam,
                           ExpressionPtr increment,
                           StatementPtr body)
  : initStmt_(std::move(initStmtParam))
  , condStmt_(std::move(condStmtParam))
  , increment_(std::move(increment))
  , body_(std::move(body))
{}

void ForStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const
{
    stream << "# ForStatement EmitRISC (stub)\n";
    if (initStmt_)  initStmt_->EmitRISC(stream, context, destReg);
    if (condStmt_)  condStmt_->EmitRISC(stream, context, destReg);
    if (body_)      body_->EmitRISC(stream, context, destReg);
    if (increment_) increment_->EmitRISC(stream, context, destReg);
}

void ForStatement::Print(std::ostream &stream) const
{
    stream << "ForStatement(\n";
    if (initStmt_) {
        stream << "  init: ";
        initStmt_->Print(stream);
        stream << "\n";
    }
    if (condStmt_) {
        stream << "  condition: ";
        condStmt_->Print(stream);
        stream << "\n";
    }
    if (increment_) {
        stream << "  increment: ";
        increment_->Print(stream);
        stream << "\n";
    }
    if (body_) {
        stream << "  body:\n    ";
        body_->Print(stream);
        stream << "\n";
    }
    stream << ")\n";
}

} // namespace ast
