#include "ast_selection_statement.hpp"
#include <iostream>

namespace ast {

//==================== IfStatement ====================//
IfStatement::IfStatement(ExpressionPtr condition, StatementPtr thenStmt, StatementPtr elseStmt)
  : condition_(std::move(condition))
  , thenStmt_(std::move(thenStmt))
  , elseStmt_(std::move(elseStmt))
{}

void IfStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
    stream << "# IfStatement EmitRISC (stub)\n";

    if (condition_) condition_->EmitRISC(stream, context, destReg);
    if (thenStmt_)  thenStmt_->EmitRISC(stream, context, destReg);
    if (elseStmt_)  elseStmt_->EmitRISC(stream, context, destReg);
}

void IfStatement::Print(std::ostream &stream) const {
    stream << "IfStatement(\n";
    if (condition_) {
        stream << "  condition: ";
        condition_->Print(stream);
        stream << "\n";
    }
    if (thenStmt_) {
        stream << "  then:\n    ";
        thenStmt_->Print(stream);
        stream << "\n";
    }
    if (elseStmt_) {
        stream << "  else:\n    ";
        elseStmt_->Print(stream);
        stream << "\n";
    }
    stream << ")\n";
}

//==================== SwitchStatement ====================//
SwitchStatement::SwitchStatement(ExpressionPtr condition, StatementPtr body)
  : condition_(std::move(condition))
  , body_(std::move(body))
{}

void SwitchStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
    stream << "# SwitchStatement EmitRISC (stub)\n";
    if (condition_) condition_->EmitRISC(stream, context, destReg);
    if (body_)      body_->EmitRISC(stream, context, destReg);
}

void SwitchStatement::Print(std::ostream &stream) const {
    stream << "SwitchStatement(\n";
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

} // namespace ast
