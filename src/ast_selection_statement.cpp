#include "ast_selection_statement.hpp"
#include <iostream>

namespace ast {

//==================== IfStatement ====================//
    void IfStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        std::string labelElse = context.MakeLabel("else");
        std::string labelEnd = context.MakeLabel("endif");

        if (condition_) {
            Register condReg = context.AllocateTemporary();
            condition_->EmitRISC(stream, context, condReg);


            stream << "beq " << condReg << ", zero, " << labelElse << "\n";

            context.FreeTemporary(condReg);
        }

        if (thenStmt_) {
            thenStmt_->EmitRISC(stream, context, destReg);
        }

        if (elseStmt_) {
            stream << "j " << labelEnd << "\n";
        }

        stream << labelElse << ":\n";
        if (elseStmt_) {
            elseStmt_->EmitRISC(stream, context, destReg);
        }

        if (elseStmt_) {
            stream << labelEnd << ":\n";
        }
    }

    void IfStatement::Print(std::ostream &stream) const {
        stream << "if (";
        if (condition_) {
            condition_->Print(stream);
        }
        stream << ")";
        if (thenStmt_) {
            thenStmt_->Print(stream);
        }
        if (elseStmt_) {
            stream << "else ";
            elseStmt_->Print(stream);
        }
    }

//==================== SwitchStatement ====================//
    void SwitchStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        stream << "# SwitchStatement (stub)\n";

        if (condition_) {
            Register condReg = context.AllocateTemporary();
            condition_->EmitRISC(stream, context, condReg);
            context.FreeTemporary(condReg);
        }

        if (body_) {
            body_->EmitRISC(stream, context, destReg);
        }
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
