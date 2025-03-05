#include "ast_multiplicative_expression.hpp"
#include "ast_unary_expression.hpp"
#include "ast_additive_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

MultiplicativeExpression::~MultiplicativeExpression() = default;

void MultiplicativeExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const
{
    if (op_ == MultiplicativeOperator::UnaryPromote) {
        right_->EmitRISC(stream, context, destReg);
        return;
    }
    Register leftReg = context.AllocateTemporary();
    left_->EmitRISC(stream, context, leftReg);
    Register rightReg = context.AllocateTemporary();
    right_->EmitRISC(stream, context, rightReg);

    switch (op_) {
        case MultiplicativeOperator::Multiply:
            stream << "mul " << destReg << "," << leftReg << "," << rightReg << std::endl;
            break;
        case MultiplicativeOperator::Divide:
            stream << "div " << destReg << "," << leftReg << "," << rightReg << std::endl;
            break;
        case MultiplicativeOperator::Modulo:
            stream << "rem " << destReg << "," << leftReg << "," << rightReg << std::endl;
            break;
        case MultiplicativeOperator::UnaryPromote:
            break;
    }
    context.FreeTemporary(leftReg);
    context.FreeTemporary(rightReg);
}

void MultiplicativeExpression::Print(std::ostream &stream) const
{
    if (op_ == MultiplicativeOperator::UnaryPromote) {
        right_->Print(stream);
        return;
    }
    if (left_) {
        left_->Print(stream);
    }
    switch (op_) {
        case MultiplicativeOperator::Multiply:
            stream << " * ";
            break;
        case MultiplicativeOperator::Divide:
            stream << " / ";
            break;
        case MultiplicativeOperator::Modulo:
            stream << " % ";
            break;
        case MultiplicativeOperator::UnaryPromote:
            break;
    }
    right_->Print(stream);
}

Type MultiplicativeExpression::GetType(Context&) const {
    return Type(TypeSpecifier::INT, true);
}

} // namespace ast
