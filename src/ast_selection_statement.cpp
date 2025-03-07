#include "ast_selection_statement.hpp"
#include <sstream>

namespace ast {

// todo if we can be bothered split this and iterative statement into different files
//==================== IfStatement ====================//
    void IfStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        std::string labelElse = context.MakeLabel("else");
        std::string labelEnd = context.MakeLabel("endif");

        Register condReg = context.AllocateTemporary();
        condition_->EmitRISC(stream, context, condReg);

        stream << "beq " << condReg << ", zero, " << labelElse << std::endl;

        context.FreeTemporary(condReg);

        if (thenStmt_) {
            thenStmt_->EmitRISC(stream, context, destReg);
        }

        if (elseStmt_) {
            stream << "j " << labelEnd << std::endl;
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
        if (body_) {
            body_->SetInSwitchScope();
            // rambling
            // hmmmm we need to impl this in all statement types technically
            // but we need all of the jumps at the start...

            // need to put children in switch scope
            // but somehow need to pull pairs of integral values and labels, i think to here, or we need to distinguish between root switch scope and child switch scope

            // how about a member on statement a vector of pairs and emit risc pushes to that member
            // avoids cluttering context for a single feature

            // then only have to buffer risc here

            // how to store constexpr reference...
            // reference wrapper carrying
            // im trying to avoid evaluating it at compile time

            // this might be better than I thought, I think the only statement impl that needs to change is labeled statement
            // FENOMENO BARIAL

            std::stringstream bodyBuffer;
            body_->EmitRISC(bodyBuffer, context, destReg);

            // Emit comparisons and jumps
            // This is actually more efficient than GCC with -O0
            Register condReg = context.AllocateTemporary();
            condition_->EmitRISC(stream, context, condReg);
            for (auto &pair : body_->GetSwitchLabelCasePairs()) {
                std::string label = pair.first;
                Register constexprReg = context.AllocateTemporary(); // Only use a temporary as there won't be fn calls in case
                pair.second.get().EmitRISC(stream, context, constexprReg);
                stream << "beq " << condReg << "," << constexprReg << "," << label << std::endl;
                context.FreeTemporary(constexprReg);
            }
            context.FreeTemporary(condReg);

            stream << bodyBuffer.rdbuf();
            // todo end label in context for break.
        }

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
