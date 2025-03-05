#include "ast_and_expression.hpp"
#include "ast_equality_expression.hpp"
#include "ast_relational_expression.hpp"
#include "ast_shift_expression.hpp"
#include "ast_additive_expression.hpp"
#include "ast_multiplicative_expression.hpp"
#include "ast_unary_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

RelationalExpression::~RelationalExpression() = default;

void RelationalExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
    if (op_ == RelationalOperator::ShiftPromote) {
        right_->EmitRISC(stream, context, destReg);
        return;
    }
    Register leftReg = context.AllocateTemporary();
    left_->EmitRISC(stream, context, leftReg);
    Register rightReg = context.AllocateTemporary();
    right_->EmitRISC(stream, context, rightReg);

    switch (op_) {
        case RelationalOperator::LessThan:
            stream << "slt " << destReg << "," << leftReg << "," << rightReg << std::endl;
            break;
        case RelationalOperator::GreaterThan:
            stream << "sgt " << destReg << "," << leftReg << "," << rightReg << std::endl;
            break;
        case RelationalOperator::LessThanOrEqual:
            stream << "sgt " << destReg << "," << leftReg << "," << rightReg << std::endl;
            stream << "seqz " << destReg << "," << destReg << std::endl;
            stream << "andi " << destReg << "," << destReg << ",0xff" << std::endl;
            break;
        case RelationalOperator::GreaterThanOrEqual:
            stream << "sle " << destReg << "," << leftReg << "," << rightReg << std::endl;
            stream << "seqz " << destReg << "," << destReg << std::endl;
            stream << "andi " << destReg << "," << destReg << ",0xff" << std::endl;
            break;
        case RelationalOperator::ShiftPromote:
            break;
    }
    context.FreeTemporary(leftReg);
    context.FreeTemporary(rightReg);
}

void RelationalExpression::Print(std::ostream &stream) const {
    if (op_ == RelationalOperator::ShiftPromote) {
        right_->Print(stream);
        return;
    }
    if (left_) {
        left_->Print(stream);
    }
    switch (op_) {
        case RelationalOperator::LessThan:
            stream << " < ";
            break;
        case RelationalOperator::GreaterThan:
            stream << " > ";
            break;
        case RelationalOperator::LessThanOrEqual:
            stream << " <= ";
            break;
        case RelationalOperator::GreaterThanOrEqual:
            stream << " >= ";
            break;
        case RelationalOperator::ShiftPromote:
            break;
    }
    right_->Print(stream);
}

Type RelationalExpression::GetType(Context &context) const {  
    return Type(TypeSpecifier::INT, true);
}

} // namespace ast
