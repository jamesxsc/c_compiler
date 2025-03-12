#include <cmath>
#include "ast_assignment_expression.hpp"
#include "ast_expression.hpp"
#include "ast_type_specifier.hpp"
#include "ast_conditional_expression.hpp"
#include "ast_multiplicative_unary_expressions.hpp"

namespace ast {

    AssignmentExpression::~AssignmentExpression() = default;

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

        TypeSpecifier type = GetType(context); // Should be LHS I think
        bool useFloat = type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE;
        Register right = context.AllocateTemporary(useFloat);
        // todo check lr order
        assignment_->EmitRISC(stream, context, right); // Order is impl. defined; execute right first (same as GCC)
        if (op_ != AssignmentOperator::Assign) {
            // todo propagate float double arrays etc logic in this block
            bool leftStored = assignment_->ContainsFunctionCall();
            Register left = leftStored ? context.AllocatePersistent(useFloat) : context.AllocateTemporary(useFloat);
            unary_->EmitRISC(stream, context, left);
            // It's not ideal to duplicate instructions from other classes but unique pointer makes this a pain
            // todo ensure changes are propagated as necessary
            // Maybe we create a RISC utils or something to actually emit RISC in both places
            // ugly but tempting
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
        std::string identifier = unary_->GetIdentifier();
        // hmmm so we can store array as a different type or a large set of types with a funny identifier in our bindings/globals
        // and somehow detect it here. or how else can we implement GetIdentifier to work. cx s
        if (context.IsArray(identifier)) {
            if (context.IsGlobal(identifier)) {
                Register indexReg = context.AllocateTemporary();
                // todo ughhh do we want to change order to match gcc more closely? probably cba
                unary_->GetArrayIndexExpression().EmitRISC(stream, context, indexReg);
                int logSize = static_cast<int>(std::log2(GetTypeSize(type))); // todo will this break for structs
                if (logSize != 0) // Save an instruction if it's a char array
                    stream << "slli " << indexReg << "," << indexReg << "," << logSize << std::endl;
                Register addrReg = context.AllocateTemporary();
                stream << "lui " << addrReg << ",%hi(" << identifier << ")" << std::endl;
                stream << "addi " << addrReg << "," << addrReg << ",%lo(" << identifier << ")" << std::endl;
                stream << "add " << addrReg << "," << addrReg << "," << indexReg << std::endl;
                context.FreeTemporary(indexReg);
                switch (type) {
                    case TypeSpecifier::INT:
                        stream << "sw " << right << ",0(" << addrReg << ")" << std::endl;
                        break;
                    case TypeSpecifier::FLOAT:
                        stream << "fsw " << right << ",0(" << addrReg << ")" << std::endl;
                        break;
                    case TypeSpecifier::DOUBLE:
                        stream << "fsd " << right << ",0(" << addrReg << ")" << std::endl;
                        break;
                    case TypeSpecifier::CHAR:
                        stream << "sb " << right << ",0(" << addrReg << ")" << std::endl;
                        break;
                    case TypeSpecifier::POINTER:
                        throw std::runtime_error(
                                "ArrayIndexExpression::EmitRISC() called on a pointer array which is unsupported");
                }
            } else {
                Register indexReg = context.AllocateTemporary();
                unary_->GetArrayIndexExpression().EmitRISC(stream, context, indexReg);
                int logSize = static_cast<int>(std::log2(GetTypeSize(type)));
                if (logSize != 0) // Save an instruction if it's a char array
                    stream << "slli " << indexReg << "," << indexReg << "," << logSize << std::endl;
                Register addrReg = context.AllocateTemporary();
                // Offset of start of array
                stream << "addi " << addrReg << ",s0," << context.CurrentFrame().bindings.Get(identifier).offset << std::endl;
                stream << "add " << addrReg << "," << addrReg << "," << indexReg << std::endl;
                context.FreeTemporary(indexReg);
                switch (type) {
                    case TypeSpecifier::INT:
                        stream << "sw " << right << ",0(" << addrReg << ")" << std::endl;
                        break;
                    case TypeSpecifier::FLOAT:
                        stream << "fsw " << right << ",0(" << addrReg << ")" << std::endl;
                        break;
                    case TypeSpecifier::DOUBLE:
                        stream << "fsd " << right << ",0(" << addrReg << ")" << std::endl;
                        break;
                    case TypeSpecifier::CHAR:
                        stream << "sb " << right << ",0(" << addrReg << ")" << std::endl;
                        break;
                    case TypeSpecifier::POINTER:
                        throw std::runtime_error(
                                "ArrayIndexExpression::EmitRISC() called on a pointer array which is unsupported");
                }
            }
        } else {
            if (context.IsGlobal(identifier)) {
                switch (type) {
                    case TypeSpecifier::FLOAT:
                    case TypeSpecifier::DOUBLE: {
                        Register tempReg = context.AllocateTemporary();
                        stream << "lui " << tempReg << ",%hi(" << identifier << ")" << std::endl;
                        stream << (type == TypeSpecifier::FLOAT ? "fsw " : "fsd ") << right << ",%lo(" << identifier
                               << ")("
                               << tempReg << ")" << std::endl;
                        context.FreeTemporary(tempReg);
                        break;
                    }
                    case TypeSpecifier::POINTER: {
                        Register tempReg = context.AllocateTemporary();
                        stream << "lui " << tempReg << ",%hi(" << identifier << ")" << std::endl;
                        stream << "sw " << right << ",%lo(" << identifier << ")(" << tempReg << ")" << std::endl;
                        context.FreeTemporary(tempReg);
                        break;
                    }
                    case TypeSpecifier::INT: {
                        Register tempReg = context.AllocateTemporary();
                        stream << "lui " << tempReg << ",%hi(" << identifier << ")" << std::endl;
                        stream << "sw " << right << ",%lo(" << identifier << ")(" << tempReg << ")" << std::endl;
                        context.FreeTemporary(tempReg);
                        break;
                    }
                    case TypeSpecifier::CHAR:
                        Register tempReg = context.AllocateTemporary();
                        stream << "lui " << tempReg << ",%hi(" << identifier << ")" << std::endl;
                        stream << "sb " << right << ",%lo(" << identifier << ")(" << tempReg << ")" << std::endl;
                        context.FreeTemporary(tempReg);
                        break;
                }
            } else {
                Variable lhsVariable = context.CurrentFrame().bindings.Get(
                        identifier); // Can only assign to lvalue so this call should succeed
                switch (type) {
                    case TypeSpecifier::FLOAT:
                    case TypeSpecifier::DOUBLE:
                        stream << (type == TypeSpecifier::FLOAT ? "fsw " : "fsd ") << right << "," << lhsVariable.offset
                               << "(s0)" << std::endl;
                        break;
                    case TypeSpecifier::INT:
                        stream << "sw " << right << "," << lhsVariable.offset << "(s0)" << std::endl;
                        break;
                    case TypeSpecifier::CHAR:
                        stream << "sb " << right << "," << lhsVariable.offset << "(s0)" << std::endl;
                        break;
                    case TypeSpecifier::POINTER:
                        // Pointer, load the address (LHS equivalent of UnaryOperator::Dereference)
                        Register addrReg = context.AllocateTemporary();
                        stream << "lw " << addrReg << "," << lhsVariable.offset << "(s0)" << std::endl;
                        stream << "sw " << right << ",0(" << addrReg << ")" << std::endl;
                        context.FreeTemporary(addrReg);
                        break;
                }
            }
        }
        // All "return" the result in destReg (if it's used)
        if (destReg != Register::zero) {
            stream << "mv " << destReg << "," << right << std::endl;
        }
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


    AssignmentExpression::AssignmentExpression(UnaryExpressionPtr unary, AssignmentOperator op,
                                               AssignmentExpressionPtr assignment) : op_(op), conditional_(nullptr),
                                                                                     unary_(std::move(unary)),
                                                                                     assignment_(
                                                                                             std::move(assignment)) {}

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

    // These are for constant expressions that will never be assignments
    std::string AssignmentExpression::GetGlobalIdentifier() const {
        return conditional_->GetGlobalIdentifier();
    }

    int AssignmentExpression::GetGlobalValue() const {
        return conditional_->GetGlobalValue();
    }

}