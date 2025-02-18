#include "ast_postfix_expression.hpp"

namespace ast {

    void PostfixExpression::EmitRISC(std::ostream &stream, Context &context) const
    {
        child_->EmitRISC(stream, context);
    }

    void PostfixExpression::Print(std::ostream &stream) const
    {
        child_->Print(stream); // Only support primary expression which we just print as is
    }

} // namespace ast
