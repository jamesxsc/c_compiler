#include <cmath>
#include <cassert>
#include "ast_assignment_expression.hpp"
#include "ast_expression.hpp"
#include "ast_type_specifier.hpp"
#include "ast_conditional_expression.hpp"
#include "ast_unary_expression.hpp"
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
                Utils::EmitMultiply(stream, context, result, *assignment_, *unary_);
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
//        if (unary_->IsPointerDereference()) { // Type is already unfolded, moved to top as this won't always have an identifier
        // I think this, is ok, although maybe we call unary gettype?
        Register addrReg = context.AllocateTemporary();
        bool restore = context.SetEmitLHS(true); // Get raw address
        unary_->EmitRISC(stream, context, addrReg);
        context.SetEmitLHS(restore);
        // Copied from below could maybe extract
        bool ptr = type.IsPointer();
        if (ptr) type = type.GetPointeeType();
        switch (type) {
            case TypeSpecifier::FLOAT:
            case TypeSpecifier::DOUBLE:
            case TypeSpecifier::POINTER:
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
            case TypeSpecifier::VOID:
            case TypeSpecifier::STRUCT:
            case TypeSpecifier::ARRAY:
                throw std::runtime_error("Unsupported type for assignment");
        }
        context.FreeTemporary(addrReg);

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

    // The logic of this function is to return the type of the actual arithmetic operation (unwraps ptrs etc.)
    // probably bin this
    TypeSpecifier AssignmentExpression::GetTypeInternal(Context &context) const {
        // Just pass down, there is only one operand
        if (op_ == AssignmentOperator::ConditionalPromote) {
            return conditional_->GetType(context);
        }

        if (unary_->IsPointerDereference()) {
            // Raw address
            return unary_->GetType(context).GetPointeeType();
        } else {
            // LHS with identifier
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
    }

    // Return expression type
    TypeSpecifier AssignmentExpression::GetType(Context &context) const {
        if (op_ == AssignmentOperator::ConditionalPromote) {
            bool restore = context.SetEmitLHS(true);
            TypeSpecifier ret =  conditional_->GetType(context);
            context.SetEmitLHS(restore);
            return ret;
        } else {
            bool restore = context.SetEmitLHS(true);
            TypeSpecifier ret =  unary_->GetType(context);
            context.SetEmitLHS(restore);
            return ret;
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

    int AssignmentExpression::Evaluate(Context &context) const {
        if (op_ == AssignmentOperator::ConditionalPromote) {
            return conditional_->Evaluate(context);
        }
        throw std::runtime_error("AssignmentExpression::Evaluate() called on an assignment expression");
    }

    double AssignmentExpression::EvaluateFloat(ast::Context &context) const {
        if (op_ == AssignmentOperator::ConditionalPromote) {
            return conditional_->EvaluateFloat(context);
        }
        throw std::runtime_error("AssignmentExpression::EvaluateFloat() called on an assignment expression");
    }

}