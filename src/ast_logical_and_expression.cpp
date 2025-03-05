#include "ast_logical_and_expression.hpp"
#include "ast_and_expression.hpp"
#include "ast_equality_expression.hpp"
#include "ast_relational_expression.hpp"
#include "ast_shift_expression.hpp"
#include "ast_additive_expression.hpp"
#include "ast_multiplicative_expression.hpp"
#include "ast_unary_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

LogicalAndExpression::~LogicalAndExpression() = default;

void LogicalAndExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
    if (!left_) {
        right_->EmitRISC(stream, context, destReg);
        return;
    }
    Register leftReg = context.AllocateTemporary();
    std::string label2 = context.MakeLabel(".L_AND");
    std::string label3 = context.MakeLabel(".L_AND");
    left_->EmitRISC(stream, context, leftReg);
    stream << "beq " << leftReg << ",zero," << label2 << std::endl;
    context.FreeTemporary(leftReg);
    Register rightReg = context.AllocateTemporary();
    right_->EmitRISC(stream, context, rightReg);
    stream << "beq " << rightReg << ",zero," << label2 << std::endl;
    context.FreeTemporary(rightReg);
    stream << "li " << destReg << ",1" << std::endl;
    stream << "j " << label3 << std::endl;
    stream << label2 << ":" << std::endl;
    stream << "li " << destReg << ",0" << std::endl;
    stream << label3 << ":" << std::endl;
}

void LogicalAndExpression::Print(std::ostream &stream) const {
    if (!left_) {
        right_->Print(stream);
        return;
    }
    left_->Print(stream);
    stream << " && ";
    right_->Print(stream);
}

Type LogicalAndExpression::GetType(Context&) const {
    return Type(TypeSpecifier::INT, true);
}

} // namespace ast
