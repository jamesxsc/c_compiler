#include "ast_assignment_expression.hpp"
#include "ast_unary_expression.hpp"
#include "ast_conditional_expression.hpp"
#include "ast_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

AssignmentExpression::~AssignmentExpression() = default;

void AssignmentExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
    switch (op_) {
        case AssignmentOperator::ConditionalPromote:
            conditional_->EmitRISC(stream, context, destReg);
            break;

        case AssignmentOperator::Assign: {
            Register lhsReg = context.AllocateTemporary();
            assignment_->EmitRISC(stream, context, lhsReg);
            Variable lhsVariable = context.CurrentFrame().bindings.at(unary_->GetIdentifier());
            stream << "sw " << lhsReg << "," << lhsVariable.offset << "(s0)" << std::endl;
            context.FreeTemporary(lhsReg);
            break;
        }

        case AssignmentOperator::MultiplyAssign:
        case AssignmentOperator::DivideAssign:
        case AssignmentOperator::ModuloAssign:
        case AssignmentOperator::AddAssign:
        case AssignmentOperator::SubtractAssign:
        case AssignmentOperator::LeftShiftAssign:
        case AssignmentOperator::RightShiftAssign:
        case AssignmentOperator::BitwiseAndAssign:
        case AssignmentOperator::BitwiseXorAssign:
        case AssignmentOperator::BitwiseOrAssign:
            std::cerr << "This assignment operator is not implemented" << std::endl;
            break;
    }
}

void AssignmentExpression::Print(std::ostream &stream) const {
    if (op_ == AssignmentOperator::ConditionalPromote) {
        conditional_->Print(stream);
        return;
    }
    unary_->Print(stream);
    switch (op_) {
        case AssignmentOperator::ConditionalPromote:
            break;
        case AssignmentOperator::Assign:
            stream << " = ";
            break;
        case AssignmentOperator::MultiplyAssign:
            stream << " *= ";
            break;
        case AssignmentOperator::DivideAssign:
            stream << " /= ";
            break;
        case AssignmentOperator::ModuloAssign:
            stream << " %= ";
            break;
        case AssignmentOperator::AddAssign:
            stream << " += ";
            break;
        case AssignmentOperator::SubtractAssign:
            stream << " -= ";
            break;
        case AssignmentOperator::LeftShiftAssign:
            stream << " <<= ";
            break;
        case AssignmentOperator::RightShiftAssign:
            stream << " >>= ";
            break;
        case AssignmentOperator::BitwiseAndAssign:
            stream << " &= ";
            break;
        case AssignmentOperator::BitwiseXorAssign:
            stream << " ^= ";
            break;
        case AssignmentOperator::BitwiseOrAssign:
            stream << " |= ";
            break;
    }
    assignment_->Print(stream);
}

AssignmentExpression::AssignmentExpression(ConditionalExpressionPtr conditional)
    : op_(AssignmentOperator::ConditionalPromote)
    , conditional_(std::move(conditional))
    , unary_(nullptr)
    , assignment_(nullptr)
{}

AssignmentExpression::AssignmentExpression(UnaryExpressionPtr unary, AssignmentOperator op,
                                           AssignmentExpressionPtr assignment)
    : op_(op)
    , conditional_(nullptr)
    , unary_(std::move(unary))
    , assignment_(std::move(assignment))
{}

Type AssignmentExpression::GetType(Context&) const {
    return Type(TypeSpecifier::INT, true);
}

} // namespace ast
