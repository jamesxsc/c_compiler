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
        // reverse order since we use s0. this can be fixed once above is addressed
        for (int i = 11; i >= 0; i--) {
            if (context.CurrentFrame().usedPersistentRegisters.test(i)) {
                stream << "lw s" << i << ", " << frameSize - 8 - i * 4 << "(sp)" << std::endl;
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
        return expression_->GetType(context);
    }
}
