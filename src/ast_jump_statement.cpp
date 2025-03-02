#include "ast_jump_statement.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void ReturnStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (expression_ != nullptr) {
            expression_->EmitRISC(stream, context, Register::a0);
        }

        // Stack/frame pointer/return address teardown
        // todo we need to decide if we allow decrementing the stack pointer mid functino or if we force ourselves to determine it before emitting the prologue
        // ideally we use sp not s0 here but if we grow the frame we can't rely on sp + size - 4/8
        int frameSize = context.CurrentFrame().size;
        stream << "lw ra," << frameSize - 4 << "(sp)" << std::endl;
        stream << "lw s0," << frameSize - 8 << "(sp)" << std::endl;

        // Temp restore temporaries (testing)
        stream << "lw t0," << frameSize - 12 << "(sp)" << std::endl;
        stream << "lw t1," << frameSize - 16 << "(sp)" << std::endl;
        stream << "lw t2," << frameSize - 20 << "(sp)" << std::endl;
        stream << "lw t3," << frameSize - 24 << "(sp)" << std::endl;
        stream << "lw t4," << frameSize - 28 << "(sp)" << std::endl;
        stream << "lw t5," << frameSize - 32 << "(sp)" << std::endl;
        stream << "lw t6," << frameSize - 36 << "(sp)" << std::endl;

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


    ast::Type ReturnStatement::GetType(Context &) const {
        return ast::Type(ast::TypeSpecifier::INT, true);
    }
}
