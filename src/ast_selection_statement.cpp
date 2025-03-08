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
// todo test for nested switch
    void SwitchStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (body_) {
            // todo delete my comments and put anything useful in the management/readme
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
            // not really poss.
            // im trying to avoid evaluating it at compile time

            // now im thinking about the lifetime of the end and start jobs.
            // we dont need to worry about unfolding nested... but inner scopes need to see outer eg if in for
            // I think in stackframe makes most sense

            // this might be better than I thought, I think the only statement impl that needs to change is labeled statement

            std::string endLabel = context.MakeLabel(".SWITCH_END");
            context.CurrentFrame().breakLabel = endLabel;

            std::stringstream bodyBuffer;
            body_->SetInSwitchScope();
            body_->EmitRISC(bodyBuffer, context, destReg);

            // Emit comparisons and jumps
            // This is actually more efficient than GCC with -O0
            Register condReg = context.AllocateTemporary();
            condition_->EmitRISC(stream, context, condReg);
            std::string defaultLabel{endLabel};
            for (auto &pair : body_->GetSwitchLabelCasePairs()) {
                if (pair.second == nullptr) {
                    defaultLabel = pair.first;
                    continue;
                }
                std::string label = pair.first;
                Register constexprReg = context.AllocateTemporary(); // Only use a temporary as there won't be fn calls in case
                pair.second->EmitRISC(stream, context, constexprReg);
                stream << "beq " << condReg << "," << constexprReg << "," << label << std::endl;
                context.FreeTemporary(constexprReg);
            }
            // Default or end
            stream << "j " << defaultLabel << std::endl;

            context.FreeTemporary(condReg);

            stream << bodyBuffer.rdbuf();

            stream << endLabel << ":" << std::endl;
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
