#include "ast_unary_expression.hpp"
#include "ast_postfix_expression.hpp"
#include "ast_multiplicative_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

UnaryExpression::~UnaryExpression() = default;

void UnaryExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
    child_->EmitRISC(stream, context, destReg);
}

void UnaryExpression::Print(std::ostream &stream) const {
    child_->Print(stream);
}

std::string UnaryExpression::GetIdentifier() const {
    return "";
}

Type UnaryExpression::GetType(Context &context) const {
    return child_->GetType(context);
}

} // namespace ast
