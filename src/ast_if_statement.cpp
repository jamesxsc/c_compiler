#include "ast_if_statement.hpp"
#include "risc_utils.hpp"
#include <sstream>

namespace ast {

    void IfStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        std::string labelElse = context.MakeLabel("else");
        std::string labelEnd = context.MakeLabel("endif");

        Register condReg = context.AllocateTemporary();
        Utils::EmitComparison(stream, context, condReg, *condition_);
        stream << "beq " << condReg << ", zero, " << labelElse << std::endl;

        context.FreeRegister(condReg);

        // Never null
        if (inSwitchScope_)
            thenStmt_->SetInSwitchScope();
        thenStmt_->EmitRISC(stream, context, destReg);
        if (inSwitchScope_) {
            LabelCasePairVector childPairs = thenStmt_->GetSwitchLabelCasePairs();
            switchLabelCasePairs_.insert(std::end(switchLabelCasePairs_), std::begin(childPairs),
                                         std::end(childPairs));
        }        if (elseStmt_) {
            stream << "j " << labelEnd << std::endl;
        }

        stream << labelElse << ":\n";
        if (elseStmt_) {
            if (inSwitchScope_)
                elseStmt_->SetInSwitchScope();
            elseStmt_->EmitRISC(stream, context, destReg);
            if (inSwitchScope_) {
                LabelCasePairVector childPairs = elseStmt_->GetSwitchLabelCasePairs();
                switchLabelCasePairs_.insert(std::end(switchLabelCasePairs_), std::begin(childPairs),
                                             std::end(childPairs));
            }
            stream << labelEnd << ":\n";
        }
    }

    void IfStatement::Print(std::ostream &stream) const {
        stream << "if (";
        condition_->Print(stream);
        stream << ") ";
        thenStmt_->Print(stream);
        if (elseStmt_) {
            stream << "else ";
            elseStmt_->Print(stream);
        }
    }

    int IfStatement::RequiredStackSpace(Context &context) const {
        return thenStmt_->RequiredStackSpace(context) + (elseStmt_ ? elseStmt_->RequiredStackSpace(context) : 0);
    }

} // namespace ast
