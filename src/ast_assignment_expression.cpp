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

    TypeSpecifier type = GetType(context);
    bool useFloat = (type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE);
    bool rightStored = (op_ != AssignmentOperator::Assign) && unary_->ContainsFunctionCall();
    Register result = rightStored ? context.AllocatePersistent(useFloat) : context.AllocateTemporary(useFloat);

    switch (op_) {
        case AssignmentOperator::ConditionalPromote:
            break;
        case AssignmentOperator::Assign:
            assignment_->EmitRISC(stream, context, result);
            break;
        case AssignmentOperator::MultiplyAssign:
            Utils::EmitMultiply(stream, context, result, *assignment_, *unary_);
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

    std::string identifier = unary_->GetIdentifier();
    if (context.IsArray(identifier)) {
        if (context.IsGlobal(identifier)) {
            Register indexReg = context.AllocateTemporary();
            unary_->GetArrayIndexExpression().EmitRISC(stream, context, indexReg);
            int logSize = static_cast<int>(std::log2(type.GetTypeSize()));
            if (logSize != 0)
                stream << "slli " << indexReg << "," << indexReg << "," << logSize << std::endl;
            Register addrReg = context.AllocateTemporary();
            stream << "lui " << addrReg << ",%hi(" << identifier << ")" << std::endl;
            stream << "addi " << addrReg << "," << addrReg << ",%lo(" << identifier << ")" << std::endl;
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
                default:
                    throw std::runtime_error("AssignmentExpression::EmitRISC() called on an unsupported array type");
            }
            context.FreeTemporary(addrReg);
        } else {
            Register indexReg = context.AllocateTemporary();
            unary_->GetArrayIndexExpression().EmitRISC(stream, context, indexReg);
            int logSize = static_cast<int>(std::log2(type.GetTypeSize()));
            if (logSize != 0)
                stream << "slli " << indexReg << "," << indexReg << "," << logSize << std::endl;
            Register addrReg = context.AllocateTemporary();
            stream << "addi " << addrReg << ",s0," << context.CurrentFrame().bindings.Get(identifier).offset << std::endl;
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
                default:
                    throw std::runtime_error("AssignmentExpression::EmitRISC() called on an unsupported array type");
            }
            context.FreeTemporary(addrReg);
        }
    } else if (unary_->IsPointerDereference()) {
        if (context.IsGlobal(identifier)) {
            Register globalPtr = context.AllocateTemporary();
            stream << "lui " << globalPtr << ",%hi(" << identifier << ")" << std::endl;
            stream << "lw " << globalPtr << ",%lo(" << identifier << ")(" << globalPtr << ")" << std::endl;
            switch (type) {
                case TypeSpecifier::FLOAT:
                case TypeSpecifier::DOUBLE:
                    stream << (type == TypeSpecifier::FLOAT ? "fsw " : "fsd ")
                           << result << ",0(" << globalPtr << ")" << std::endl;
                    break;
                case TypeSpecifier::UNSIGNED:
                case TypeSpecifier::INT:
                    stream << "sw " << result << ",0(" << globalPtr << ")" << std::endl;
                    break;
                case TypeSpecifier::CHAR:
                    stream << "sb " << result << ",0(" << globalPtr << ")" << std::endl;
                    break;
                default:
                    throw std::runtime_error("Unsupported type for assignment to global pointer dereference");
            }
            context.FreeTemporary(globalPtr);
        } else {
            Register addrReg = context.AllocateTemporary();
            stream << "lw " << addrReg << "," << context.CurrentFrame().bindings.Get(identifier).offset << "(s0)" << std::endl;
            switch (type) {
                case TypeSpecifier::FLOAT:
                case TypeSpecifier::DOUBLE:
                    stream << (type == TypeSpecifier::FLOAT ? "fsw " : "fsd ")
                           << result << ",0(" << addrReg << ")" << std::endl;
                    break;
                case TypeSpecifier::UNSIGNED:
                case TypeSpecifier::INT:
                    stream << "sw " << result << ",0(" << addrReg << ")" << std::endl;
                    break;
                case TypeSpecifier::CHAR:
                    stream << "sb " << result << ",0(" << addrReg << ")" << std::endl;
                    break;
                default:
                    throw std::runtime_error("Unsupported type for assignment");
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
                    stream << (type == TypeSpecifier::FLOAT ? "fsw " : "fsd ")
                           << result << ",%lo(" << identifier << ")(" << tempReg << ")" << std::endl;
                    context.FreeTemporary(tempReg);
                    break;
                }
                case TypeSpecifier::POINTER: {
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
                default:
                    throw std::runtime_error("Unsupported type for global assignment");
            }
        } else {
            Variable lhsVariable = context.CurrentFrame().bindings.Get(identifier);
            switch (type) {
                case TypeSpecifier::FLOAT:
                case TypeSpecifier::DOUBLE:
                    stream << (type == TypeSpecifier::FLOAT ? "fsw " : "fsd ")
                           << result << "," << lhsVariable.offset << "(s0)" << std::endl;
                    break;
                case TypeSpecifier::UNSIGNED:
                case TypeSpecifier::INT:
                    stream << "sw " << result << "," << lhsVariable.offset << "(s0)" << std::endl;
                    break;
                case TypeSpecifier::CHAR:
                    stream << "sb " << result << "," << lhsVariable.offset << "(s0)" << std::endl;
                    break;
                case TypeSpecifier::POINTER:
                    stream << "sw " << result << "," << lhsVariable.offset << "(s0)" << std::endl;
                    break;
                default:
                    throw std::runtime_error("Unsupported type for assignment");
            }
        }
    }

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

AssignmentExpression::AssignmentExpression(ConditionalExpressionPtr conditional)
    : op_(AssignmentOperator::ConditionalPromote),
      conditional_(std::move(conditional)),
      unary_(nullptr),
      assignment_(nullptr) {}

AssignmentExpression::AssignmentExpression(UnaryExpressionPtr unary, AssignmentOperator op,
                                           AssignmentExpressionPtr assignment)
    : op_(op),
      conditional_(nullptr),
      unary_(std::move(unary)),
      assignment_(std::move(assignment)) {}

TypeSpecifier AssignmentExpression::GetType(Context &context) const {
    if (op_ == AssignmentOperator::ConditionalPromote) {
        return conditional_->GetType(context);
    }
    std::string identifier = unary_->GetIdentifier();
    TypeSpecifier lhsType = context.IsGlobal(identifier)
        ? context.GetGlobalType(identifier)
        : context.CurrentFrame().bindings.Get(identifier).type;
    if (lhsType.IsArray()) {
        lhsType = lhsType.GetArrayType();
    }
    if (lhsType.IsPointer() && unary_->IsPointerDereference()) {
        lhsType = lhsType.GetPointeeType();
    }
    return lhsType;
}

bool AssignmentExpression::ContainsFunctionCall() const {
    if (op_ == AssignmentOperator::ConditionalPromote) {
        return conditional_->ContainsFunctionCall();
    } else {
        return unary_->ContainsFunctionCall() || assignment_->ContainsFunctionCall();
    }
}

std::string AssignmentExpression::GetGlobalIdentifier() const {
    return conditional_->GetGlobalIdentifier();
}

int AssignmentExpression::GetGlobalValue() const {
    return conditional_->GetGlobalValue();
}

int AssignmentExpression::Evaluate() const {
    throw std::runtime_error("AssignmentExpression::Evaluate() called on an assignment expression");
}

} // namespace ast
