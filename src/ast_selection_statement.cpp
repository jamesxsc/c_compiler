#include "ast_selection_statement.hpp"
#include <iostream>

namespace ast {

// todo if we can be bothered split this and iterative statement into different files
//==================== IfStatement ====================//
    void IfStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        std::string labelElse = context.MakeLabel("else");
        std::string labelEnd = context.MakeLabel("endif");

        Register condReg = context.AllocateTemporary();
        condition_->EmitRISC(stream, context, condReg);

        stream << "beq " << condReg << ", zero, " << labelElse << "\n";

        context.FreeTemporary(condReg);

        if (thenStmt_) {
            thenStmt_->EmitRISC(stream, context, destReg);
        }

        if (elseStmt_) {
            stream << "j " << labelEnd << "\n";
        }

        stream << labelElse << ":\n";
        if (elseStmt_) {
            elseStmt_->EmitRISC(stream, context, destReg);
            stream << labelEnd << ":\n";
        }
    }

    void IfStatement::Print(std::ostream &stream) const {
        stream << "if (";
        condition_->Print(stream);
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
        Register condReg = context.AllocateTemporary();
        condition_->EmitRISC(stream, context, condReg);

        if (body_) {
            body_->EmitRISC(stream, context, destReg);
        }

        context.FreeTemporary(condReg);
    }

    void SwitchStatement::Print(std::ostream &stream) const {
        stream << "switch (";
        condition_->Print(stream);
        stream << ")";

        if (body_) {
            body_->Print(stream);
        }
    }

} // namespace ast
