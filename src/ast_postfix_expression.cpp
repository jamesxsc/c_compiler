#include "ast_postfix_expression.hpp"
#include "ast_identifier.hpp"

namespace ast {

    void PostfixExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        child_->EmitRISC(stream, context, destReg);
    }

    void PostfixExpression::Print(std::ostream &stream) const {
        child_->Print(stream); // Only support primary expression which we just print as is
    }

    std::string PostfixExpression::GetIdentifier() const {
        // todo handle array/member case
        Identifier identifier = dynamic_cast<const Identifier&>(*child_);
        return identifier.GetIdentifier();
    }

} // namespace ast
