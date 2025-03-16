#include <cmath>
#include <cassert>
#include "ast_assignment_expression.hpp"
#include "ast_expression.hpp"
#include "ast_type_specifier.hpp"
#include "ast_conditional_expression.hpp"
#include "ast_multiplicative_unary_expressions.hpp"
#include "risc_utils.hpp"

namespace ast {

    AssignmentExpression::~AssignmentExpression() = default;

    void AssignmentExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (op_ == AssignmentOperator::ConditionalPromote) {
            conditional_->EmitRISC(stream, context, destReg);
            return;
        }

        TypeSpecifier type = GetType(context); // unfolded LHS type
        bool useFloat = type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE;
        bool rightStored = (op_ != AssignmentOperator::Assign) &&
                           unary_->ContainsFunctionCall(); // I can't see any case where this is true
        Register result = rightStored ? context.AllocatePersistent(useFloat) : context.AllocateTemporary(useFloat);
        switch (op_) {
            case AssignmentOperator::ConditionalPromote:
                break; // Handled above
            case AssignmentOperator::Assign:
                assignment_->EmitRISC(stream, context, result);
                break;
            case AssignmentOperator::MultiplyAssign:
                Utils::EmitMultiply(stream, context, result, *assignment_,*unary_);
                // Invert order like GCC, unary less likely to need storing
                break;
            case AssignmentOperator::DivideAssign:
                Utils::EmitDivide(stream, context, result, *unary_, *assignment_);
                break;
            case AssignmentOperator::ModuloAssign:
                assert(!useFloat && "Modulo assignment operator not supported for floating point types");
                Utils::EmitModulo(stream, context, result, *unary_, *assignment_);
                break;
            case AssignmentOperator::AddAssign:
                Utils::EmitAddition(stream, context, result, *assignment_, *unary_);
                break;
            case AssignmentOperator::SubtractAssign:
                Utils::EmitSubtraction(stream, context, result, *unary_, *assignment_);
                break;
            case AssignmentOperator::LeftShiftAssign:
                assert(!useFloat && "Left shift assignment operator not supported for floating point types");
                Utils::EmitLeftShift(stream, context, result, *unary_, *assignment_);
                break;
            case AssignmentOperator::RightShiftAssign:
                assert(!useFloat && "Right shift assignment operator not supported for floating point types");
                Utils::EmitRightShift(stream, context, result, *unary_, *assignment_);
                break;
            case AssignmentOperator::BitwiseAndAssign:
                assert(!useFloat && "Bitwise AND assignment operator not supported for floating point types");
                Utils::EmitBitwiseAnd(stream, context, result, *assignment_, *unary_);
                break;
            case AssignmentOperator::BitwiseXorAssign:
                assert(!useFloat && "Bitwise XOR assignment operator not supported for floating point types");
                Utils::EmitBitwiseXor(stream, context, result, *assignment_, *unary_);
                break;
            case AssignmentOperator::BitwiseOrAssign:
                assert(!useFloat && "Bitwise OR assignment operator not supported for floating point types");
                Utils::EmitBitwiseOr(stream, context, result, *assignment_, *unary_);
                break;
        }

        // Common: store the result
        std::string identifier = unary_->GetIdentifier();
        if (context.IsArray(identifier)) {
            if (context.IsGlobal(identifier)) {
                Register indexReg = context.AllocateTemporary();
                // todo ughhh do we want to change order to match gcc more closely? probably cba
                unary_->GetArrayIndexExpression().EmitRISC(stream, context, indexReg);
                int logSize = static_cast<int>(std::log2(type.GetTypeSize())); // todo will this break for structs
                if (logSize != 0) // Save an instruction if it's a char array
                    stream << "slli " << indexReg << "," << indexReg << "," << logSize << std::endl;
                Register addrReg = context.AllocateTemporary();
                stream << "lui " << addrReg << ",%hi(" << identifier << ")" << std::endl;
                stream << "addi " << addrReg << "," << addrReg << ",%lo(" << identifier << ")" << std::endl;
                stream << "add " << addrReg << "," << addrReg << "," << indexReg << std::endl;
                context.FreeTemporary(indexReg);
                switch (type) {
                    case TypeSpecifier::UNSIGNED:
                    case TypeSpecifier::INT:
                    case TypeSpecifier::ENUM:
                        stream << "sw " << result << ",0(" << addrReg << ")" << std::endl;
                        break;
                    case TypeSpecifier::FLOAT:
                        stream << "fsw " << result << ",0(" << addrReg << ")" << std::endl;
                        break;
                    case TypeSpecifier::DOUBLE:
                        stream << "fsd " << result << ",0(" << addrReg << ")" << std::endl;
                        break;
                    case TypeSpecifier::CHAR:
                        stream << "sb " << result << ",0(" << addrReg << ")" << std::endl;
                        break;
                    case TypeSpecifier::POINTER:
                    case TypeSpecifier::VOID:
                    case TypeSpecifier::STRUCT:
                    case TypeSpecifier::ARRAY:
                        throw std::runtime_error(
                                "AssignmentExpression::EmitRISC() called on an unsupported array type");
                        // todo these do need to be supported
                }
                context.FreeTemporary(addrReg);
            } else {
                Register indexReg = context.AllocateTemporary();
                unary_->GetArrayIndexExpression().EmitRISC(stream, context, indexReg);
                int logSize = static_cast<int>(std::log2(type.GetTypeSize()));
                if (logSize != 0) // Save an instruction if it's a char array
                    stream << "slli " << indexReg << "," << indexReg << "," << logSize << std::endl;
                Register addrReg = context.AllocateTemporary();
                // Offset of start of array
                stream << "addi " << addrReg << ",s0," << context.CurrentFrame().bindings.Get(identifier).offset
                       << std::endl;
                stream << "add " << addrReg << "," << addrReg << "," << indexReg << std::endl;
                context.FreeTemporary(indexReg);
                switch (type) {
                    case TypeSpecifier::UNSIGNED:
                    case TypeSpecifier::INT:
                        stream << "sw " << result << ",0(" << addrReg << ")" << std::endl;
                        break;
                    case TypeSpecifier::FLOAT:
                        stream << "fsw " << result << ",0(" << addrReg << ")" << std::endl;
                        break;
                    case TypeSpecifier::DOUBLE:
                        stream << "fsd " << result << ",0(" << addrReg << ")" << std::endl;
                        break;
                    case TypeSpecifier::CHAR:
                        stream << "sb " << result << ",0(" << addrReg << ")" << std::endl;
                        break;
                    case TypeSpecifier::POINTER:
                    case TypeSpecifier::VOID:
                    case TypeSpecifier::ENUM:
                    case TypeSpecifier::STRUCT:
                    case TypeSpecifier::ARRAY:
                        throw std::runtime_error(
                                "AssignmentExpression::EmitRISC() called on an unsupported array type");
                }
                context.FreeTemporary(addrReg);
            }
        } else if (unary_->IsPointerDereference()) { // Type is already unfolded
            // replaces the deref'd check inside pointer case - that will only be reassigning a pointer or ptr to ptr
            // this is actually a really nice way to avoid nested switches and capture different underlying type stores
            // am i missing something - where else do we do this? nowehere?
            // its not the same as de ref because that's reading not writing
            // use temp reg to obtain address - should be common to global/non global
            if (context.IsGlobal(identifier)) {
                // todo global ptrs
            } else {
                Register addrReg = context.AllocateTemporary();
                stream << "lw " << addrReg << "," << context.CurrentFrame().bindings.Get(identifier).offset << "(s0)"
                          << std::endl;
                // Copied from below could maybe extract
                switch (type) {
                    case TypeSpecifier::FLOAT:
                    case TypeSpecifier::DOUBLE:
                        stream << (type == TypeSpecifier::FLOAT ? "fsw " : "fsd ") << result << ",0(" << addrReg << ")"
                               << std::endl;
                        break;
                    case TypeSpecifier::UNSIGNED:
                    case TypeSpecifier::INT:
                    case TypeSpecifier::ENUM:
                        stream << "sw " << result << ",0(" << addrReg << ")" << std::endl;
                        break;
                    case TypeSpecifier::CHAR:
                        stream << "sb " << result << ",0(" << addrReg << ")" << std::endl;
                        break;
                    case TypeSpecifier::POINTER:
                    case TypeSpecifier::VOID:
                    case TypeSpecifier::STRUCT:
                    case TypeSpecifier::ARRAY:
                        throw std::runtime_error("Unsupported type for assignment");
                        // TODO need to support at least some of these
                }
                context.FreeTemporary(addrReg);
            }
        } else {
            if (context.IsGlobal(identifier)) {
                switch (type) {
                    case TypeSpecifier::FLOAT:
                    case TypeSpecifier::DOUBLE: {
                        Register tempReg = context.AllocateTemporary();
                        stream << "lui " << tempReg << ",%hi(" << identifier << ")" << std::endl;
                        stream << (type == TypeSpecifier::FLOAT ? "fsw " : "fsd ") << result << ",%lo(" << identifier
                               << ")("
                               << tempReg << ")" << std::endl;
                        context.FreeTemporary(tempReg);
                        break;
                    }
                    case TypeSpecifier::POINTER: {
                        // Reassign the pointer
                        Register tempReg = context.AllocateTemporary();
                        stream << "lui " << tempReg << ",%hi(" << identifier << ")" << std::endl;
                        stream << "sw " << result << ",%lo(" << identifier << ")(" << tempReg << ")" << std::endl;
                        context.FreeTemporary(tempReg);
                        break;
                    }
                    case TypeSpecifier::UNSIGNED:
                    case TypeSpecifier::INT: {
                        Register tempReg = context.AllocateTemporary();
                        stream << "lui " << tempReg << ",%hi(" << identifier << ")" << std::endl;
                        stream << "sw " << result << ",%lo(" << identifier << ")(" << tempReg << ")" << std::endl;
                        context.FreeTemporary(tempReg);
                        break;
                    }
                    case TypeSpecifier::CHAR: {
                        Register tempReg = context.AllocateTemporary();
                        stream << "lui " << tempReg << ",%hi(" << identifier << ")" << std::endl;
                        stream << "sb " << result << ",%lo(" << identifier << ")(" << tempReg << ")" << std::endl;
                        context.FreeTemporary(tempReg);
                        break;
                    }
                    case TypeSpecifier::VOID:
                    case TypeSpecifier::ENUM:
                    case TypeSpecifier::STRUCT:
                    case TypeSpecifier::ARRAY:
                        throw std::runtime_error("Unsupported type for global assignment");
                        // TODO need to support at least some of these
                }
            } else {
                Variable lhsVariable = context.CurrentFrame().bindings.Get(identifier);
                // Can only assign to lvalue so this call should succeed
                switch (type) {
                    case TypeSpecifier::FLOAT:
                    case TypeSpecifier::DOUBLE:
                        stream << (type == TypeSpecifier::FLOAT ? "fsw " : "fsd ") << result << ","
                               << lhsVariable.offset << "(s0)" << std::endl;
                        break;
                    case TypeSpecifier::UNSIGNED:
                    case TypeSpecifier::INT:
                    case TypeSpecifier::ENUM:
                        stream << "sw " << result << "," << lhsVariable.offset << "(s0)" << std::endl;
                        break;
                    case TypeSpecifier::CHAR:
                        stream << "sb " << result << "," << lhsVariable.offset << "(s0)" << std::endl;
                        break;
                    case TypeSpecifier::POINTER: {
                        // Reassign the pointer
                        stream << "sw " << result << "," << lhsVariable.offset << "(s0)" << std::endl;
                        break;
                    }
                    case TypeSpecifier::VOID:
                    case TypeSpecifier::STRUCT:
                    case TypeSpecifier::ARRAY:
                        throw std::runtime_error("Unsupported type for assignment");
                        // TODO need to support at least some of these
                }
            }
        }
        // All "return" the result in destReg (if it's used)
        if (destReg != Register::zero) {
            stream << "mv " << destReg << "," << result << std::endl;
        }
        context.FreeTemporary(result);
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

    // The logic of this function is to return the type of the actual arithmetic operation
    TypeSpecifier AssignmentExpression::GetType(Context &context) const {
        // Just pass down, there is only one operand
        if (op_ == AssignmentOperator::ConditionalPromote) {
            return conditional_->GetType(context);
        }

        // LHS will always have an identifier
        std::string identifier = unary_->GetIdentifier();
        TypeSpecifier lhsType = context.IsGlobal(identifier) ?
                                context.GetGlobalType(identifier) :
                                context.CurrentFrame().bindings.Get(identifier).type;
        if (lhsType.IsArray())
            lhsType = lhsType.GetArrayType(); // Will always be indexed
        if (lhsType.IsPointer() && unary_->IsPointerDereference())
            lhsType = lhsType.GetPointeeType();

        return lhsType;
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

    int AssignmentExpression::Evaluate(Context &context) const {
        if (op_ == AssignmentOperator::ConditionalPromote) {
            return conditional_->Evaluate(context);
        }
        throw std::runtime_error("AssignmentExpression::Evaluate() called on an assignment expression");
    }

}