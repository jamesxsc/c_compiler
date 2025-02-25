#include "ast_jump_statement.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

void ReturnStatement::EmitRISC(std::ostream& stream, Context& context, Register destReg) const
{
    if (expression_ != nullptr)
    {
        expression_->EmitRISC(stream, context, Register::a0);
    }

    // Stack/frame pointer/return address teardown
    stream << "lw ra," << context.CurrentFrame().size - 4 << "(sp)" << std::endl;
    stream << "lw s0," << context.CurrentFrame().size - 8 << "(sp)" << std::endl;
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


    ast::Type ReturnStatement::GetType(Context&) const {
        return ast::Type(ast::TypeSpecifier::INT, true);
    }
}
