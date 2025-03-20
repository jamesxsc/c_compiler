#include <cmath>
#include "risc_utils.hpp"

namespace ast::Utils {

    void EmitComparison(std::ostream &stream, Context &context, Register result, const ExpressionBase &operand) {
        switch (operand.GetType(context)) {
            case TypeSpecifier::Type::INT:
            case TypeSpecifier::Type::POINTER:
            case TypeSpecifier::Type::UNSIGNED:
            case TypeSpecifier::Type::ARRAY:
            case TypeSpecifier::Type::ENUM:
            case TypeSpecifier::Type::CHAR:
                operand.EmitRISC(stream, context, result);
                break;
            case TypeSpecifier::Type::FLOAT: {
                Register tempReg1 = context.AllocateTemporary(stream, true);
                Register tempReg2 = context.AllocateTemporary(stream, true);
                operand.EmitRISC(stream, context, tempReg2);
                stream << "fmv.s.x " << tempReg1 << ",zero" << std::endl;
                stream << "feq.s " << result << "," << tempReg2 << "," << tempReg1 << std::endl;
                stream << "seqz " << result << "," << result << std::endl;
                context.FreeTemporary(tempReg1, stream);
                context.FreeTemporary(tempReg2, stream);
                break;
            }
            case TypeSpecifier::Type::DOUBLE: {
                Register tempReg1 = context.AllocateTemporary(stream, true);
                Register tempReg2 = context.AllocateTemporary(stream, true);
                operand.EmitRISC(stream, context, tempReg2);
                stream << "fcvt.d.w " << tempReg1 << ",zero" << std::endl;
                stream << "feq.d " << result << "," << tempReg2 << "," << tempReg1 << std::endl;
                stream << "seqz " << result << "," << result << std::endl;
                break;
            }
            case TypeSpecifier::Type::STRUCT:
            case TypeSpecifier::Type::VOID:
                throw std::runtime_error("LogicalOrExpression::EmitComparison() called on an unsupported type");
        }
    }

    void EmitMultiply(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                      const ExpressionBase &right) {
        TypeSpecifier type = BinaryResultType(left.GetType(context), right.GetType(context));
        // at the very least for ptr arithmetic we will need to check both sides
        bool useFloat = type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE;
        bool leftStored = right.ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent(useFloat) : context.AllocateTemporary(
                stream, useFloat);
        left.EmitRISC(stream, context, leftReg);
//        Register rightReg = context.AllocateTemporary(stream, useFloat);
        Register rightReg = result;
        right.EmitRISC(stream, context, rightReg);

        switch (type) {
            case TypeSpecifier::FLOAT:
            case TypeSpecifier::DOUBLE:
                stream << (type == TypeSpecifier::FLOAT ? "fmul.s " : "fmul.d ") << result << "," << leftReg << ","
                       << rightReg << std::endl;
                break;
            case TypeSpecifier::INT:
            case TypeSpecifier::UNSIGNED:
            case TypeSpecifier::CHAR:
            case TypeSpecifier::ENUM:
                stream << "mul " << result << "," << leftReg << "," << rightReg << std::endl;
                break;
            case TypeSpecifier::POINTER:
            case TypeSpecifier::ARRAY:
            case TypeSpecifier::STRUCT:
            case TypeSpecifier::VOID:
                throw std::runtime_error("Multiplicative operation attempted on unsupported type.");
        }

        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg, stream);
//        context.FreeTemporary(rightReg, stream);
    }

    void EmitDivide(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                    const ExpressionBase &right) {
        TypeSpecifier type = BinaryResultType(left.GetType(context), right.GetType(context));
        bool useFloat = type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE;
        bool leftStored = right.ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent(useFloat) : context.AllocateTemporary(
                stream, useFloat);
        left.EmitRISC(stream, context, leftReg);
        Register rightReg = result;
        right.EmitRISC(stream, context, rightReg);

        switch (type) {
            case TypeSpecifier::FLOAT:
            case TypeSpecifier::DOUBLE:
                stream << (type == TypeSpecifier::FLOAT ? "fdiv.s " : "fdiv.d ") << result << "," << leftReg << ","
                       << rightReg << std::endl;
                break;
            case TypeSpecifier::INT:
            case TypeSpecifier::UNSIGNED:
            case TypeSpecifier::CHAR:
            case TypeSpecifier::ENUM:
                stream << "div " << result << "," << leftReg << "," << rightReg << std::endl;
                break;
            case TypeSpecifier::POINTER:
            case TypeSpecifier::ARRAY:
            case TypeSpecifier::STRUCT:
            case TypeSpecifier::VOID:
                throw std::runtime_error("Multiplicative operation attempted on unsupported type.");
        }

        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg, stream);
    }

    void EmitModulo(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                    const ExpressionBase &right) {
        TypeSpecifier type = BinaryResultType(left.GetType(context), right.GetType(context));
        bool useFloat = type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE;
        bool leftStored = right.ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent(useFloat) : context.AllocateTemporary(
                stream, useFloat);
        left.EmitRISC(stream, context, leftReg);
        Register rightReg = result;
        right.EmitRISC(stream, context, rightReg);

        switch (type) {
            case TypeSpecifier::INT:
            case TypeSpecifier::ENUM: // GCC uses remu, but only as an optimisation
                stream << "rem " << result << "," << leftReg << "," << rightReg << std::endl;
                break;
            case TypeSpecifier::CHAR: // todo there is masking here, at least when int is result check!
            case TypeSpecifier::UNSIGNED: // This is after promotion so no need to check signs
                stream << "remu " << result << "," << leftReg << "," << rightReg << std::endl;
                break;
            default:
                throw std::runtime_error("Multiplicative operation attempted on unsupported type.");
        }

        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg, stream);
    }

    void EmitAddition(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                      const ExpressionBase &right) {
        TypeSpecifier type = BinaryAdditionResultType(left.GetType(context), right.GetType(context));
        bool leftStored = right.ContainsFunctionCall();
        bool useFloat = type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE;
        Register leftReg = leftStored ? context.AllocatePersistent(useFloat) : context.AllocateTemporary(
                stream, useFloat);
        left.EmitRISC(stream, context, leftReg);
        Register rightReg = result;
        right.EmitRISC(stream, context, rightReg);
        switch (type) {
            case TypeSpecifier::FLOAT:
                stream << "fadd.s " << result << "," << leftReg << "," << rightReg << std::endl;
                break;
            case TypeSpecifier::DOUBLE:
                stream << "fadd.d " << result << "," << leftReg << "," << rightReg << std::endl;
                break;
            case TypeSpecifier::POINTER:
            case TypeSpecifier::ARRAY: {
                // We are guaranteed here that only one is a pointer
                if (left.GetType(context).IsPointer()) {
                    Utils::EmitIndexToAddressOffset(stream, rightReg, context, left.GetType(context).GetPointeeType());
                } else {
                    Utils::EmitIndexToAddressOffset(stream, leftReg, context, right.GetType(context).GetPointeeType());
                }
                stream << "add " << result << "," << leftReg << "," << rightReg << std::endl;
                break;
            }
            case TypeSpecifier::INT:
            case TypeSpecifier::UNSIGNED:
            case TypeSpecifier::CHAR:
            case TypeSpecifier::ENUM:
                stream << "add " << result << "," << leftReg << "," << rightReg << std::endl;
                break;
            case TypeSpecifier::STRUCT:
            case TypeSpecifier::VOID:
                throw std::runtime_error("Attempted addition on an unsupported type.");
        }
        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg, stream);
    }

    void EmitSubtraction(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                         const ExpressionBase &right) {
        TypeSpecifier type = BinarySubtractionResultType(left.GetType(context), right.GetType(context));
        // Arithmetic type is different from return type for ptr - ptr
        if (left.GetType(context).IsPointer() && right.GetType(context).IsPointer())
            type = left.GetType(context);
        bool leftStored = right.ContainsFunctionCall();
        bool useFloat = type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE;
        Register leftReg = leftStored ? context.AllocatePersistent(useFloat) : context.AllocateTemporary(
                stream, useFloat);
        left.EmitRISC(stream, context, leftReg);
        Register rightReg = result;
        right.EmitRISC(stream, context, rightReg);
        switch (type) {
            case TypeSpecifier::FLOAT:
                stream << "fsub.s " << result << "," << leftReg << "," << rightReg << std::endl;
                break;
            case TypeSpecifier::DOUBLE:
                stream << "fsub.d " << result << "," << leftReg << "," << rightReg << std::endl;
                break;
            case TypeSpecifier::ARRAY:
            case TypeSpecifier::POINTER: {
                // We are guaranteed here that both are ptrs
                stream << "sub " << result << "," << leftReg << "," << rightReg << std::endl;
                Utils::EmitAddressToIndexOffset(stream, result, context, left.GetType(context).GetPointeeType());
                break;
            }
            case TypeSpecifier::INT:
            case TypeSpecifier::UNSIGNED:
            case TypeSpecifier::CHAR:
            case TypeSpecifier::ENUM:
                stream << "sub " << result << "," << leftReg << "," << rightReg << std::endl;
                break;
            case TypeSpecifier::STRUCT:
            case TypeSpecifier::VOID:
                throw std::runtime_error("Attempted subtraction on an unsupported type.");
        }
        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg, stream);
    }

    void EmitLeftShift(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                       const ExpressionBase &right) {
        bool leftStored = right.ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent() : context.AllocateTemporary(stream);
        left.EmitRISC(stream, context, leftReg);
        Register rightReg = result;
        right.EmitRISC(stream, context, rightReg);
        stream << "sll " << result << "," << leftReg << "," << rightReg << std::endl;
        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg, stream);
    }

    void EmitRightShift(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                        const ExpressionBase &right) {
        bool leftStored = right.ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent() : context.AllocateTemporary(stream);
        left.EmitRISC(stream, context, leftReg);
        Register rightReg = result;
        right.EmitRISC(stream, context, rightReg);
        if (left.GetType(context).IsSigned())
            stream << "sra " << result << "," << leftReg << "," << rightReg << std::endl;
        else
            stream << "srl " << result << "," << leftReg << "," << rightReg << std::endl;
        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg, stream);
    }

    void EmitBitwiseAnd(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                        const ExpressionBase &right) {
        bool leftStored = right.ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent() : context.AllocateTemporary(stream);
        left.EmitRISC(stream, context, leftReg);
        Register rightReg = result;
        right.EmitRISC(stream, context, rightReg);
        stream << "and " << result << "," << leftReg << "," << rightReg << std::endl;
        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg, stream);
    }

    void EmitBitwiseXor(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                        const ExpressionBase &right) {
        bool leftStored = right.ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent() : context.AllocateTemporary(stream);
        left.EmitRISC(stream, context, leftReg);
        Register rightReg = result;
        right.EmitRISC(stream, context, rightReg);
        stream << "xor " << result << "," << leftReg << "," << rightReg << std::endl;
        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg, stream);
    }

    void EmitBitwiseOr(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                       const ExpressionBase &right) {
        bool leftStored = right.ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent() : context.AllocateTemporary(stream);
        left.EmitRISC(stream, context, leftReg);
        Register rightReg = result;
        right.EmitRISC(stream, context, rightReg);
        stream << "or " << result << "," << leftReg << "," << rightReg << std::endl;
        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg, stream);
    }

    // This is good enough unless we get failures as a direct result
    TypeSpecifier BinaryResultType(const TypeSpecifier &leftType, const TypeSpecifier &rightType) {
        // Choose wider type, and choose unsigned if equal
        if (leftType.GetTypeSize() > rightType.GetTypeSize()) {
            return leftType;
        } else if (leftType.GetTypeSize() == rightType.GetTypeSize()) {
            if (leftType.IsSigned() && !rightType.IsSigned())
                return rightType;
            else
                // Always promote
                return leftType == TypeSpecifier::CHAR ? TypeSpecifier{TypeSpecifier::UNSIGNED} : leftType;
        } else {
            return rightType;
        }
    }

    // TODO handle array type here - should only be for pointer treating in add/sub
    // but where does it get passed, identifier? need tests ASAP
    TypeSpecifier BinaryAdditionResultType(const TypeSpecifier &leftType, const TypeSpecifier &rightType) {
        // Choose wider type, and choose unsigned if equal
        // Differs from other operators because of ptr arithmetic
        if (leftType.IsPointer() != rightType.IsPointer()) {
            if (leftType.IsPointer())
                return leftType;
            else
                return rightType;
        } else if (leftType.IsPointer() && rightType.IsPointer()) {
            throw std::runtime_error("Pointer addition not supported");
        }

        return BinaryResultType(leftType, rightType);
    }

    TypeSpecifier BinarySubtractionResultType(const TypeSpecifier &leftType, const TypeSpecifier &rightType) {
        // Choose wider type, and choose unsigned if equal
        // Differs from other operators because of ptr arithmetic
        if (leftType.IsPointer() != rightType.IsPointer()) {
            if (leftType.IsPointer())
                return leftType;
            else
                return rightType;
        } else if (leftType.IsPointer() && rightType.IsPointer()) {
            return TypeSpecifier::INT; // ABI says its signed, and this will only be used on integrals
        }

        return BinaryResultType(leftType, rightType);
    }

    // Quite efficient, and works for structs
    void EmitIndexToAddressOffset(std::ostream &stream, Register sizeReg, Context &context, const TypeSpecifier &type) {
        int elementSize = type.GetTypeSize();
        if (elementSize == 1) return;
        bool pow2 = (elementSize & (elementSize - 1)) == 0;
        if (pow2) {
            int logSize = static_cast<int>(std::log2(elementSize));
            stream << "slli " << sizeReg << "," << sizeReg << "," << logSize << std::endl;
        } else {
            Register temp = context.AllocateTemporary(stream);
            stream << "li " << temp << "," << elementSize << std::endl;
            stream << "mul " << sizeReg << "," << sizeReg << "," << temp << std::endl;
            context.FreeTemporary(temp, stream);
        }
    }

    void EmitAddressToIndexOffset(std::ostream &stream, Register sizeReg, Context &context, const TypeSpecifier &type) {
        int elementSize = type.GetTypeSize();
        if (elementSize == 1) return;
        bool pow2 = (elementSize & (elementSize - 1)) == 0;
        if (pow2) {
            int logSize = static_cast<int>(std::log2(elementSize));
            stream << "srai " << sizeReg << "," << sizeReg << "," << logSize << std::endl;
        } else {
            Register temp = context.AllocateTemporary(stream);
            stream << "li " << temp << "," << elementSize << std::endl;
            stream << "div " << sizeReg << "," << sizeReg << "," << temp << std::endl;
            context.FreeTemporary(temp, stream);
        }
    }

    void EmitIncrementDecrement(std::ostream &stream, Context &context, Register destReg, const ExpressionBase &child,
                                bool decrement, bool postfix) {
        Context::ScopedEmitLHS guard(context, true); // Get raw address
        TypeSpecifier lhsType = child.GetType(context);
        Register addrReg = context.AllocateTemporary(stream);
        TypeSpecifier type = child.GetType(context);
        child.EmitRISC(stream, context, addrReg);
        guard.Release();

        if (destReg != Register::zero && postfix)
            child.EmitRISC(stream, context, destReg);

        // Now safely increment
        bool ptr = type.IsPointer();
        if (ptr) type = type.GetPointeeType();
        bool useFloat = type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE;
        Register tempReg = (postfix || destReg == Register::zero) ? context.AllocateTemporary(stream, useFloat)
                                                                  : destReg;
        switch (type) {
            case TypeSpecifier::Type::INT:
            case TypeSpecifier::Type::CHAR:
            case TypeSpecifier::Type::UNSIGNED:
            case TypeSpecifier::Type::ENUM: {
                stream << (type == TypeSpecifier::CHAR ? "lbu " : "lw ") << tempReg << ","
                       << "0(" << addrReg << ")" << std::endl;
                stream << "addi " << tempReg << "," << tempReg << ","
                       << (decrement ? -1 : 1) << std::endl;
                stream << (type == TypeSpecifier::CHAR ? "sb " : "sw ") << tempReg << ","
                       << "0(" << addrReg << ")" << std::endl;
                break;
            }
            case TypeSpecifier::Type::FLOAT:
            case TypeSpecifier::Type::DOUBLE: {
                stream << (type == TypeSpecifier::FLOAT ? "flw " : "fld ") << tempReg
                       << ","
                       << "0(" << addrReg << ")" << std::endl;
                std::string constantMemoryLabel = context.MakeLabel("POSTFIX_CONSTANT");
                Register tempAddrReg = context.AllocateTemporary(stream);
                Register tempFloatReg2 = context.AllocateTemporary(stream, true);
                stream << "lui " << tempAddrReg << ",%hi(" << constantMemoryLabel << ")" << std::endl;
                stream << (type == TypeSpecifier::FLOAT ? "flw " : "fld ") << tempFloatReg2
                       << ",%lo("
                       << constantMemoryLabel << ")(" << tempAddrReg << ")" << std::endl;
                if (!decrement) {
                    stream << (type == TypeSpecifier::FLOAT ? "fadd.s " : "fadd.d ")
                           << tempReg
                           << "," << tempReg << "," << tempFloatReg2 << std::endl;
                } else {
                    stream << (type == TypeSpecifier::FLOAT ? "fsubd.s " : "fsub.d ")
                           << tempReg
                           << "," << tempReg << "," << tempFloatReg2 << std::endl;
                }
                stream << (type == TypeSpecifier::FLOAT ? "fsw " : "fsd ") << tempReg
                       << ","
                       << "0(" << addrReg << ")" << std::endl;
                context.FreeTemporary(tempAddrReg, stream);
                context.FreeTemporary(tempFloatReg2, stream);

                context.DeferredRISC() << ".section .rodata" << std::endl;
                context.DeferredRISC() << ".align "
                                       << (type == TypeSpecifier::Type::FLOAT ? 2 : 3)
                                       << std::endl;
                context.DeferredRISC() << constantMemoryLabel << ":" << std::endl;
                context.DeferredRISC()
                        << (type == TypeSpecifier::Type::FLOAT ? ".float " : ".double ")
                        << (decrement ? -1.0 : 1.0)
                        << std::endl;
                break;
            }
            case TypeSpecifier::Type::POINTER:
            case TypeSpecifier::Type::ARRAY: {
                const TypeSpecifier &pointeePointeeType = type.GetPointeeType();
                stream << "lw " << tempReg << "," << "0(" << addrReg << ")" << std::endl;
                int size = pointeePointeeType.GetTypeSize();
                stream << "addi " << tempReg << "," << tempReg << ","
                       << (decrement ? -size : size) << std::endl;
                stream << "sw " << tempReg << "," << "0(" << addrReg << ")" << std::endl;
                break;
            }
            case TypeSpecifier::Type::VOID:
            case TypeSpecifier::Type::STRUCT:
                throw std::runtime_error(
                        "PostfixExpression::EmitRISC() attempted to increment unsupported type");
        }
        context.FreeTemporary(addrReg, stream);
        if (tempReg != destReg) context.FreeTemporary(tempReg, stream);
        if (!postfix && destReg != Register::zero) {
            child.EmitRISC(stream, context, destReg);
        }
    }

}
