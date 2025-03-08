#include "ast_assignment_expression.hpp"
#include "ast_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {


    void AssignmentExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (op_ == AssignmentOperator::ConditionalPromote) {
            conditional_->EmitRISC(stream, context, destReg);
            return;
        }

        // todo if it is already in a reg AND (BIG IF) WE DECIDE TO STORE REGISTERS/VARIABLES IN CONTEXT THEN UPDATE THAT
        // come back to this as a maybe
        // todo diff types and arrays will be different here
        // this is for int
        // todo i dont like how we don't always call unary emit risc (minor issue)

        Register right = context.AllocateTemporary();
        assignment_->EmitRISC(stream, context, right); // Order is impl. defined; execute right first (same as GCC)
        if (op_ != AssignmentOperator::Assign) {
            bool leftStored = assignment_->ContainsFunctionCall();
            Register left = leftStored ? context.AllocatePersistent() : context.AllocateTemporary();
            unary_->EmitRISC(stream, context, left);
            // It's not ideal to duplicate instructions from other classes but unique pointer makes this a pain
            // todo ensure changes are propagated as necessary
            // Maybe we create a RISC utils or something to actually emit RISC in both places
            // ugly.
            // how about instantiate the correct thing in the parser?
            // layers do make hard
            switch (op_) {
                case AssignmentOperator::ConditionalPromote: // Should never happen
                case AssignmentOperator::Assign: // Should never happen
                    break;
                case AssignmentOperator::MultiplyAssign:
                    stream << "mul " << right << "," << left << "," << right << std::endl;
                    break;
                case AssignmentOperator::DivideAssign:
                    stream << "div " << right << "," << left << "," << right << std::endl;
                    break;
                case AssignmentOperator::ModuloAssign:
                    // Checked, this logic is correct
                    if (IsSigned(assignment_->GetType(context)) && IsSigned(unary_->GetType(context)))
                        stream << "rem " << right << "," << left << "," << right << std::endl;
                    else
                        stream << "remu " << right << "," << left << "," << right << std::endl;
                    break;
                case AssignmentOperator::AddAssign:
                    stream << "add " << right << "," << left << "," << right << std::endl;
                    break;
                case AssignmentOperator::SubtractAssign:
                    stream << "sub " << right << "," << left << "," << right << std::endl;
                    break;
                case AssignmentOperator::LeftShiftAssign:
                    stream << "sll " << right << "," << left << "," << right << std::endl;
                    break;
                case AssignmentOperator::RightShiftAssign:
                    if (IsSigned(unary_->GetType(context)))
                        stream << "sra " << right << "," << left << "," << right << std::endl;
                    else
                        stream << "srl " << right << "," << left << "," << right << std::endl;
                    break;
                case AssignmentOperator::BitwiseAndAssign:
                    stream << "and " << right << "," << left << "," << right << std::endl;
                    break;
                case AssignmentOperator::BitwiseXorAssign:
                    stream << "xor " << right << "," << left << "," << right << std::endl;
                    break;
                case AssignmentOperator::BitwiseOrAssign:
                    stream << "or " << right << "," << left << "," << right << std::endl;
                    break;
            }
            leftStored ? context.FreePersistent(left) : context.FreeTemporary(left);
        }
        // Common: store the result
        // Can only assign to lvalue so this call should succeed
        Variable lhsVariable = context.CurrentFrame().bindings.Get(unary_->GetIdentifier());
        if (unary_->GetType(context) == TypeSpecifier::POINTER) {
            // Pointer, load the address (LHS equivalent of UnaryOperator::Dereference)
            Register addrReg = context.AllocateTemporary();
            stream << "lw " << addrReg << "," << lhsVariable.offset << "(s0)" << std::endl;
            stream << "sw " << right << ",0(" << addrReg << ")" << std::endl;
            context.FreeTemporary(addrReg);
        } else {
            stream << "sw " << right << "," << lhsVariable.offset << "(s0)" << std::endl;
        }
        // All "return" the result in destReg
        stream << "mv " << destReg << "," << right << std::endl;
        context.FreeTemporary(right);
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

    AssignmentExpression::AssignmentExpression(ConditionalExpressionPtr
                                               conditional) : op_(
            AssignmentOperator::ConditionalPromote), conditional_(std::move(conditional)), unary_(
            nullptr), assignment_(nullptr) {}

    AssignmentExpression::AssignmentExpression(UnaryExpressionPtr unary, AssignmentOperator op, AssignmentExpressionPtr assignment) : op_(op), conditional_(nullptr), unary_(std::move(unary)), assignment_(std::move(assignment)) {}


    TypeSpecifier AssignmentExpression::GetType(Context &context) const {
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