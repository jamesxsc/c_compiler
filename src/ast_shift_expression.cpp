#include "ast_and_expression.hpp"
#include "ast_equality_expression.hpp"
#include "ast_relational_expression.hpp"
#include "ast_shift_expression.hpp"
#include "ast_additive_expression.hpp"
#include "ast_multiplicative_expression.hpp"
#include "ast_unary_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

ShiftExpression::~ShiftExpression() = default;

void ShiftExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
    if (op_ == ShiftOperator::AdditivePromote) {
        right_->EmitRISC(stream, context, destReg);
        return;
    }
    Register leftReg = context.AllocateTemporary();
    left_->EmitRISC(stream, context, leftReg);
    Register rightReg = context.AllocateTemporary();
    right_->EmitRISC(stream, context, rightReg);

    switch (op_) {
        case ShiftOperator::Left:
            stream << "sll " << destReg << "," << leftReg << "," << rightReg << std::endl;
            break;
        case ShiftOperator::Right: {
            Type leftType = left_->GetType(context);
            bool isSigned = leftType.IsSigned();
            if (isSigned) {
                stream << "sra " << destReg << "," << leftReg << "," << rightReg << std::endl;
            } else {
                stream << "srl " << destReg << "," << leftReg << "," << rightReg << std::endl;
            }
            break;
        }
        case ShiftOperator::AdditivePromote:
            break;
    }
    context.FreeTemporary(leftReg);
    context.FreeTemporary(rightReg);
}

void ShiftExpression::Print(std::ostream &stream) const {
    if (op_ == ShiftOperator::AdditivePromote) {
        right_->Print(stream);
        return;
    }
    if (left_) {
        left_->Print(stream);
    }
    switch (op_) {
        case ShiftOperator::Left:
            stream << " << ";
            break;
        case ShiftOperator::Right:
            stream << " >> ";
            break;
        case ShiftOperator::AdditivePromote:
            break;
    }
    right_->Print(stream);
}

Type ShiftExpression::GetType(Context &context) const {  
    if (op_ == ShiftOperator::AdditivePromote) {
        return right_->GetType(context);
    }
    return Type(TypeSpecifier::INT, true);
}

} // namespace ast
