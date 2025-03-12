#include "ast_iteration_statement.hpp"
#include <iostream>

namespace ast {

//==================== WhileStatement ====================//
    void WhileStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        std::string labelStart = context.MakeLabel("while_start");
        std::string labelEnd = context.MakeLabel("while_end");
        context.CurrentFrame().breakLabel = labelEnd;
        context.CurrentFrame().continueLabel = labelStart;

        stream << labelStart << ":" << std::endl;

        // Evaluate condition
        Register condReg = context.AllocateTemporary();
        condition_->EmitRISC(stream, context, condReg);

        // If condReg == 0 => jump labelEnd
        stream << "beq " << condReg << ", zero, " << labelEnd << std::endl;
        context.FreeTemporary(condReg);

        // Never null
        body_->EmitRISC(stream, context, destReg);

        // Jump back
        stream << "j " << labelStart << std::endl;

        // label_end:
        stream << labelEnd << ":" << std::endl;
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
        context.CurrentFrame().breakLabel = labelEnd;
        context.CurrentFrame().continueLabel = labelStart;

        stream << labelStart << ":" << std::endl;
        body_->EmitRISC(stream, context, destReg);

        Register condReg = context.AllocateTemporary();
        condition_->EmitRISC(stream, context, condReg);

        stream << "bne " << condReg << ", zero, " << labelStart << std::endl;

        context.FreeTemporary(condReg);

        stream << labelEnd << ":" << std::endl;
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
        std::string labelEnd = context.MakeLabel("for_end");
        context.CurrentFrame().breakLabel = labelEnd;
        context.CurrentFrame().continueLabel = labelStart;

        initStmt_->EmitRISC(stream, context, destReg);

        stream << labelStart << ":" << std::endl;

        Register condReg = context.AllocateTemporary();
        condStmt_->EmitRISC(stream, context, condReg);

        stream << "beq " << condReg << ", zero, " << labelEnd << std::endl;
        context.FreeTemporary(condReg);

        body_->EmitRISC(stream, context, destReg);

        if (increment_) {
            Register incReg = context.AllocateTemporary();
            increment_->EmitRISC(stream, context, incReg);
            context.FreeTemporary(incReg);
        }

        stream << "j " << labelStart << std::endl;

        stream << labelEnd << ":" << std::endl;
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
