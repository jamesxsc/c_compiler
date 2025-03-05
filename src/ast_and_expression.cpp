#include "ast_and_expression.hpp"
#include "ast_equality_expression.hpp"
#include "ast_relational_expression.hpp"
#include "ast_shift_expression.hpp"
#include "ast_additive_expression.hpp"
#include "ast_multiplicative_expression.hpp"
#include "ast_unary_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

AndExpression::~AndExpression() = default;

void AndExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
    if (!left_) {
        right_->EmitRISC(stream, context, destReg);
        return;
    }
    Register leftReg = context.AllocateTemporary();
    left_->EmitRISC(stream, context, leftReg);
    Register rightReg = context.AllocateTemporary();
    right_->EmitRISC(stream, context, rightReg);
    stream << "and " << destReg << "," << leftReg << "," << rightReg << std::endl;
    context.FreeTemporary(leftReg);
    context.FreeTemporary(rightReg);
}

void AndExpression::Print(std::ostream &stream) const {
    if (left_) {
        left_->Print(stream);
        stream << " & ";
    }
    right_->Print(stream);
}

Type AndExpression::GetType(Context&) const {
    return Type(TypeSpecifier::INT, true);
}

} // namespace ast
