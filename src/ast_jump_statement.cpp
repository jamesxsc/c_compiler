#include <cassert>
#include "ast_jump_statement.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void ReturnStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // todo I mentioned a return label and moving all this to the actual function emitter.
        // this would make typing this easier and wouldnt fuck up for return 0; in a float function
        Register returnReg =
                (GetType(context) == TypeSpecifier::FLOAT ||
                 GetType(context) == TypeSpecifier::DOUBLE)
                ? Register::fa0 : Register::a0;

        if (expression_ != nullptr) {
            expression_->EmitRISC(stream, context, returnReg);
        }

        int frameSize = context.CurrentFrame().size;

        // Stack/frame pointer/return address teardown
        stream << "lw ra," << frameSize - 4 << "(sp)" << std::endl;

        // Restore used persistent registers
        int storedCount = 0;
        for (int r = 0; r < 12; r++) {
            if (context.CurrentFrame().usedIntegerPersistentRegisters.test(r)) {
                stream << "lw s" << r << "," << frameSize - 8 - storedCount * 4 << "(sp)" << std::endl;
                storedCount++;
            }
        }
        storedCount++; // Make space for 64 bit reg // todo check this is correct and cx declaration
        for (int r = 0; r < 8; r++) {
            if (context.CurrentFrame().usedFloatPersistentRegisters.test(r)) {
                stream << "flw fs" << r << "," << frameSize - 8 - storedCount * 4 << "(sp)" << std::endl;
                storedCount += 2; // Float registers are 64 bit
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
