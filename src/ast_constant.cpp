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

    bool IntConstant::ContainsFunctionCall() const {
        return false;
    }


} // namespace ast
