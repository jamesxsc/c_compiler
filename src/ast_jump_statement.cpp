#include "ast_jump_statement.hpp"

namespace ast {

void ReturnStatement::EmitRISC(std::ostream& stream, Context& context, Register destReg) const
{
    if (expression_ != nullptr)
    {
        expression_->EmitRISC(stream, context, Register::a0);
    }

    // Stack/frame pointer/return address teardown
    // todo we need to decide if we allow decrementing the stack pointer mid functino or if we force ourselves to determine it before emitting the prologue
    // ideally we use sp not s0 here but if we grow the frame we can't rely on sp + size - 4/8
    stream << "lw ra," << -4 << "(s0)" << std::endl;
    stream << "lw s0," << -8 << "(s0)" << std::endl;
    stream << "addi sp,sp," << context.CurrentFrame().size << std::endl;

    stream << "ret" << std::endl;
}

void ReturnStatement::Print(std::ostream& stream) const
{
    stream << "return";
    if (expression_ != nullptr)
    {
        stream << " ";
        expression_->Print(stream);
    }
    stream << ";" << std::endl;
}

}
