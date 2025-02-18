#include "ast_unary_expression.hpp"

namespace ast {

    void UnaryExpression::EmitRISC(std::ostream &stream, Context &context) const {
        child_->EmitRISC(stream, context);
    }

    void UnaryExpression::Print(std::ostream &stream) const {
        child_->Print(stream);
    }

} // namespace ast
