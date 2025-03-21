#include "ast_iteration_statement.hpp"
#include "risc_utils.hpp"
#include <iostream>

namespace ast {

//==================== WhileStatement ====================//
    void WhileStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        std::string labelStart = context.MakeLabel("while_start");
        std::string labelEnd = context.MakeLabel("while_end");
        context.CurrentFrame().breakLabel.push_back(labelEnd);
        context.CurrentFrame().continueLabel.push_back(labelStart);

        stream << labelStart << ":" << std::endl;

        // Evaluate condition
        Register condReg = context.AllocateTemporary();
        Utils::EmitComparison(stream, context, condReg, *condition_);

        // If condReg == 0 => jump labelEnd
        stream << "beq " << condReg << ", zero, " << labelEnd << std::endl;
        context.FreeRegister(condReg);

        // Never null
        if (inSwitchScope_)
            body_->SetInSwitchScope();
        body_->EmitRISC(stream, context, destReg);
        if (inSwitchScope_) {
            LabelCasePairVector childPairs = body_->GetSwitchLabelCasePairs();
            switchLabelCasePairs_.insert(std::end(switchLabelCasePairs_), std::begin(childPairs),
                                         std::end(childPairs));
        }

        // Jump back
        stream << "j " << labelStart << std::endl;

        // label_end:
        stream << labelEnd << ":" << std::endl;

        context.CurrentFrame().breakLabel.pop_back();
        context.CurrentFrame().continueLabel.pop_back();
    }

    void WhileStatement::Print(std::ostream &stream) const {
        stream << "while (";
        condition_->Print(stream);
        stream << ") ";
        body_->Print(stream);
    }

//==================== DoWhileStatement ====================//
    void DoWhileStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        std::string labelStart = context.MakeLabel("do_while_start");
        std::string labelEnd = context.MakeLabel("do_while_end");
        context.CurrentFrame().breakLabel.push_back(labelEnd);
        context.CurrentFrame().continueLabel.push_back(labelStart);

        stream << labelStart << ":" << std::endl;
        // Never null
        if (inSwitchScope_)
            body_->SetInSwitchScope();
        body_->EmitRISC(stream, context, destReg);
        if (inSwitchScope_) {
            LabelCasePairVector childPairs = body_->GetSwitchLabelCasePairs();
            switchLabelCasePairs_.insert(std::end(switchLabelCasePairs_), std::begin(childPairs),
                                         std::end(childPairs));
        }

        Register condReg = context.AllocateTemporary();
        Utils::EmitComparison(stream, context, condReg, *condition_);
        stream << "bne " << condReg << ", zero, " << labelStart << std::endl;

        context.FreeRegister(condReg);

        stream << labelEnd << ":" << std::endl;

        context.CurrentFrame().continueLabel.pop_back();
        context.CurrentFrame().breakLabel.pop_back();
    }

    void DoWhileStatement::Print(std::ostream &stream) const {
        stream << "do ";
        body_->Print(stream);
        stream << "while (";
        condition_->Print(stream);
        stream << ");" << std::endl;
    }

//==================== ForStatement ====================//
    void ForStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        std::string labelStart = context.MakeLabel("for_start");
        std::string labelContinue = context.MakeLabel("for_continue");
        std::string labelEnd = context.MakeLabel("for_end");
        context.CurrentFrame().breakLabel.push_back(labelEnd);
        context.CurrentFrame().continueLabel.push_back(labelContinue);

        initStmt_->EmitRISC(stream, context, destReg);
        stream << labelStart << ":" << std::endl;

        Register condReg = context.AllocateTemporary();
        const Expression *cond = condStmt_->GetExpression();
        if (cond) {
            Utils::EmitComparison(stream, context, condReg, *cond);
        } else {
            stream << "li " << condReg << ",1" << std::endl;
        }
        stream << "beq " << condReg << ",zero," << labelEnd << std::endl;
        context.FreeRegister(condReg);

        // Never null
        if (inSwitchScope_)
            body_->SetInSwitchScope();
        body_->EmitRISC(stream, context, destReg);
        if (inSwitchScope_) {
            LabelCasePairVector childPairs = body_->GetSwitchLabelCasePairs();
            switchLabelCasePairs_.insert(std::end(switchLabelCasePairs_), std::begin(childPairs),
                                         std::end(childPairs));
        }

        stream << labelContinue << ":" << std::endl; // Must still inc
        if (increment_) {
            Register incReg = context.AllocateTemporary();
            increment_->EmitRISC(stream, context, incReg);
            context.FreeRegister(incReg);
        }

        stream << "j " << labelStart << std::endl;
        stream << labelEnd << ":" << std::endl;

        context.CurrentFrame().breakLabel.pop_back();
        context.CurrentFrame().continueLabel.pop_back();
    }

    void ForStatement::Print(std::ostream &stream) const {
        stream << "for (";
        initStmt_->Print(stream);
        condStmt_->Print(stream);
        if (increment_)
            increment_->Print(stream);
        stream << ") ";
        body_->Print(stream);
    }

} // namespace ast
