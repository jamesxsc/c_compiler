#include "ast_initializer.hpp"

namespace ast {

void Initializer::EmitRISC(std::ostream& stream, Context& context, Register destReg) const {
    if (expression_)
        expression_->EmitRISC(stream, context, destReg);
}

void Initializer::Print(std::ostream& stream) const {
    if (expression_)
        expression_->Print(stream);
}

}
