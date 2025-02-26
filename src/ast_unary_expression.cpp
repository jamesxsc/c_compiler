#include "ast_unary_expression.hpp"

namespace ast {

    void UnaryExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        child_->EmitRISC(stream, context, destReg);
    }

    void UnaryExpression::Print(std::ostream &stream) const {
        child_->Print(stream);
    }

    // We don't use references since at a lower level there may be a cast leaving a dangling reference
    std::string UnaryExpression::GetIdentifier() const {
        return child_->GetIdentifier();
    }

} // namespace ast
