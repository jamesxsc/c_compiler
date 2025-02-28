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
    // We'll do:
    //   label_start:
    //     condReg = condition
    //     beq condReg, $zero, label_end
    //     body
    //     j label_start
    //   label_end:

    std::string labelStart = context.MakeLabel("while_start");
    std::string labelEnd   = context.MakeLabel("while_end");

    // label_start:
    stream << labelStart << ":\n";

    // Evaluate condition
    if (condition_) {
        Register condReg = context.AllocateTemporary();
        condition_->EmitRISC(stream, context, condReg);

        // If condReg == 0 => jump labelEnd
        stream << "beq " << condReg << ", zero, " << labelEnd << "\n";

        context.FreeTemporary(condReg);
    }

    // Emit body
    if (body_) {
        // Use the destReg parameter
        body_->EmitRISC(stream, context, destReg);
    }

    // Jump back
    stream << "j " << labelStart << "\n";

    // label_end:
    stream << labelEnd << ":\n";
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

    std::string labelStart = context.MakeLabel("do_while_start");


    stream << labelStart << ":\n";

    if (body_) {
        body_->EmitRISC(stream, context, destReg);
    }

    if (condition_) {
        Register condReg = context.AllocateTemporary();
        condition_->EmitRISC(stream, context, condReg);


        stream << "bne " << condReg << ", zero, " << labelStart << "\n";

        context.FreeTemporary(condReg);
    }
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
ForStatement::ForStatement(ExpressionStatementPtr initStmt,
                           ExpressionStatementPtr condStmt,
                           ExpressionPtr increment,
                           StatementPtr body)
  : initStmt_(std::move(initStmt))
  , condStmt_(std::move(condStmt))
  , increment_(std::move(increment))
  , body_(std::move(body))
{}

void ForStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const
{

    std::string labelStart = context.MakeLabel("for_start");
    std::string labelEnd   = context.MakeLabel("for_end");

    if (initStmt_) {
        initStmt_->EmitRISC(stream, context, destReg);
    }

    stream << labelStart << ":\n";

    if (condStmt_) {
        Register condReg = context.AllocateTemporary();
        condStmt_->EmitRISC(stream, context, condReg);

        stream << "beq " << condReg << ", zero, " << labelEnd << "\n";
        context.FreeTemporary(condReg);
    }


    if (body_) {
        body_->EmitRISC(stream, context, destReg);
    }

    if (increment_) {
        Register incReg = context.AllocateTemporary();
        increment_->EmitRISC(stream, context, incReg);
        context.FreeTemporary(incReg);
    }

    stream << "j " << labelStart << "\n";

    stream << labelEnd << ":\n";
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
