#include "ast_constant.hpp"

namespace ast {

void IntConstant::EmitRISC(std::ostream& stream, Context&, Register destReg) const
{
    stream << "li " << destReg << "," << value_ << std::endl;
}

void IntConstant::Print(std::ostream& stream) const
{
    stream << value_;
}

} // namespace ast
