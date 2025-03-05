#include "ast_assignment_expression.hpp"
#include "ast_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {


    void AssignmentExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // TODO implement other assignment operators
        switch (op_) {

            case AssignmentOperator::ConditionalPromote:
                conditional_->EmitRISC(stream, context, destReg);
                break;
            case AssignmentOperator::Assign: {
                // We need a register for lhs // TODO do we want to check if it's already in a register?
                // also this is common between this and the other operators
                Register lhsReg = context.AllocateTemporary();

                // todo diff types and arrays will be different here
                // this is for int
                assignment_->EmitRISC(stream, context, lhsReg);

                // todo if it is already in a reg AND (BIG IF) WE DECIDE TO STORE REGISTERS/VARIABLES IN CONTEXT THEN UPDATE THAT
                // come back to this as a maybe

                // todo i dont like how we don't always call unary emit risc

                // Can only assign to lvalue so this call should succeed
                Variable lhsVariable = context.CurrentFrame().bindings.Get(unary_->GetIdentifier());
                if (unary_->GetType(context) == TypeSpecifier::POINTER) {
                    // Pointer, load the address (LHS equivalent of UnaryOperator::Derference)
                    Register addrReg = context.AllocateTemporary();
                    stream << "lw " << addrReg << "," << lhsVariable.offset << "(s0)" << std::endl;
                    stream << "sw " << lhsReg << ",0(" << addrReg << ")" << std::endl;
                    context.FreeTemporary(addrReg);
                } else {
                    stream << "sw " << lhsReg << "," << lhsVariable.offset << "(s0)"   << std::endl;
                }


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
        } else {
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
    }

    AssignmentExpression::AssignmentExpression(ConditionalExpressionPtr conditional) : op_(
            AssignmentOperator::ConditionalPromote), conditional_(std::move(conditional)), unary_(
            nullptr), assignment_(nullptr) {}

    AssignmentExpression::AssignmentExpression(UnaryExpressionPtr unary, AssignmentOperator op,
                                               AssignmentExpressionPtr assignment) : op_(op), conditional_(nullptr),
                                                                                     unary_(std::move(unary)),
                                                                                     assignment_(
                                                                                             std::move(assignment)) {}



    TypeSpecifier AssignmentExpression::GetType(Context& context) const {
        if (op_ == AssignmentOperator::ConditionalPromote) {
            return conditional_->GetType(context);
        } else {
            return unary_->GetType(context);
        }
    }

    bool AssignmentExpression::ContainsFunctionCall() const {
        if (op_ == AssignmentOperator::ConditionalPromote) {
            return conditional_->ContainsFunctionCall();
        } else {
            return unary_->ContainsFunctionCall() || assignment_->ContainsFunctionCall();
        }
    }

}