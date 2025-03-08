#include "ast_iteration_statement.hpp"
#include <iostream>

namespace ast {

//==================== WhileStatement ====================//
    void WhileStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // We'll do:
        //   label_start:
        //     condReg = condition
        //     beq condReg, $zero, label_end
        //     body
        //     j label_start
        //   label_end:

        std::string labelStart = context.MakeLabel("while_start");
        std::string labelEnd = context.MakeLabel("while_end");
        context.CurrentFrame().breakLabel = labelEnd;
        context.CurrentFrame().continueLabel = labelStart;

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

    void WhileStatement::Print(std::ostream &stream) const {
        stream << "while (";
        if (condition_) {
            condition_->Print(stream);
        }
        stream << ")\n";
        if (body_) {
            body_->Print(stream);
        }
    }

//==================== DoWhileStatement ====================//
    void DoWhileStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        std::string labelStart = context.MakeLabel("do_while_start");
        std::string labelEnd = context.MakeLabel("do_while_end");
        context.CurrentFrame().breakLabel = labelEnd;
        context.CurrentFrame().continueLabel = labelStart;

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

        stream << labelEnd << ":\n";
    }

    void DoWhileStatement::Print(std::ostream &stream) const {
        stream << "do";
        if (body_) {
            body_->Print(stream);
        }
        if (condition_) {
            stream << "while (";
            condition_->Print(stream);
            stream << ")";
        }
    }

//==================== ForStatement ====================//
    void ForStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        std::string labelStart = context.MakeLabel("for_start");
        std::string labelEnd = context.MakeLabel("for_end");
        context.CurrentFrame().breakLabel = labelEnd;
        context.CurrentFrame().continueLabel = labelStart;

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

    void ForStatement::Print(std::ostream &stream) const {
        stream << "for (";
        if (initStmt_) {
            initStmt_->Print(stream);
        }
        if (condStmt_) {
            condStmt_->Print(stream);
        }
        if (increment_) {
            increment_->Print(stream);
        }
        stream << ")\n";
        if (body_) {
            body_->Print(stream);
        }
    }

} // namespace ast
