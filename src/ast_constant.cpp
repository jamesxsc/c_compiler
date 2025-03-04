#include "ast_constant.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

void IntConstant::EmitRISC(std::ostream& stream, Context&, Register destReg) const
{
    stream << "li " << destReg << "," << value_ << std::endl;
}

void IntConstant::Print(std::ostream& stream) const
{
    stream << value_;
}


void FloatConstant::EmitRISC(std::ostream& stream, Context&, Register destReg) const
{
    int bits = *reinterpret_cast<const int*>(&value_);
    stream << "li " << destReg << "," << bits << std::endl;
}

void FloatConstant::Print(std::ostream& stream) const
{
    stream << value_;
}

} // namespace ast