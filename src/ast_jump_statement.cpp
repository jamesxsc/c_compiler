#include <cassert>
#include "ast_jump_statement.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void ReturnStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (expression_ != nullptr) {
            expression_->EmitRISC(stream, context, Register::a0);
        }

        int frameSize = context.CurrentFrame().size;

        // Stack/frame pointer/return address teardown
        stream << "lw ra," << frameSize - 4 << "(sp)" << std::endl;

        // Restore used persistent registers
        int storedCount = 0;
        for (int r = 0; r < 12; r++) {
            if (context.CurrentFrame().usedPersistentRegisters.test(r)) {
                stream << "lw s" << r << ", " << frameSize - 8 - storedCount * 4 << "(sp)" << std::endl;
                storedCount++;
            }
        }

        stream << "addi sp,sp," << context.CurrentFrame().size << std::endl;
        stream << "ret" << std::endl;
    }

    void ReturnStatement::Print(std::ostream &stream) const {
        stream << "return";
        if (expression_ != nullptr) {
            stream << " ";
            expression_->Print(stream);
        }
        stream << ";" << std::endl;
    }


    TypeSpecifier ReturnStatement::GetType(Context &context) const {
        if (expression_)
            return expression_->GetType(context);
        else
            return TypeSpecifier::INT; // todo fuck that
    }

    void BreakStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        assert(context.CurrentFrame().breakLabel && "Break statement outside of loop or switch");
        stream << "j " << *context.CurrentFrame().breakLabel << std::endl;
    }

    void BreakStatement::Print(std::ostream &stream) const {
        stream << "break;" << std::endl;
    }

    void ContinueStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        assert(context.CurrentFrame().continueLabel && "Continue statement outside of loop");
        stream << "j " << *context.CurrentFrame().continueLabel << std::endl;
    }

    void ContinueStatement::Print(std::ostream &stream) const {
        stream << "continue;" << std::endl;
    }
}
