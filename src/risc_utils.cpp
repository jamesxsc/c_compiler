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
                Register tempReg1 = context.AllocateTemporary(true);
                Register tempReg2 = context.AllocateTemporary(true);
                operand.EmitRISC(stream, context, tempReg2);
                stream << "fmv.s.x " << tempReg1 << ",zero" << std::endl;
                stream << "feq.s " << result << "," << tempReg2 << "," << tempReg1 << std::endl;
                context.FreeTemporary(tempReg1);
                context.FreeTemporary(tempReg2);
                break;
            }
            case TypeSpecifier::Type::DOUBLE: {
                Register tempReg1 = context.AllocateTemporary(true);
                Register tempReg2 = context.AllocateTemporary(true);
                operand.EmitRISC(stream, context, tempReg2);
                stream << "fcvt.d.w " << tempReg1 << ",zero" << std::endl;
                stream << "feq.d " << result << "," << tempReg2 << "," << tempReg1 << std::endl;
                break;
            }
            case TypeSpecifier::Type::STRUCT:
            case TypeSpecifier::Type::VOID:
                throw std::runtime_error("LogicalOrExpression::EmitComparison() called on an unsupported type");
        }
    }

    // todo nice to have : extract a binaryexpression class to handle common logic
    void EmitMultiply(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                      const ExpressionBase &right) {
        TypeSpecifier type = BinaryResultType(left.GetType(context), right.GetType(context));
        // at the very least for ptr arithmetic we will need to check both sides
        bool useFloat = type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE;
        bool leftStored = right.ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent(useFloat) : context.AllocateTemporary(useFloat);
        left.EmitRISC(stream, context, leftReg);
        Register rightReg = context.AllocateTemporary(useFloat);
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
                stream << "mul " << result << "," << leftReg << "," << rightReg << std::endl;
                break;
            default:
                throw std::runtime_error("Multiplicative operation attempted on unsupported type.");
        }

        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg);
        context.FreeTemporary(rightReg);
    }

    void EmitDivide(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                    const ExpressionBase &right) {
        TypeSpecifier type = BinaryResultType(left.GetType(context), right.GetType(context));
        bool useFloat = type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE;
        bool leftStored = right.ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent(useFloat) : context.AllocateTemporary(useFloat);
        left.EmitRISC(stream, context, leftReg);
        Register rightReg = context.AllocateTemporary(useFloat);
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
                stream << "div " << result << "," << leftReg << "," << rightReg << std::endl;
                break;
            default:
                throw std::runtime_error("Multiplicative operation attempted on unsupported type.");
        }

        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg);
        context.FreeTemporary(rightReg);
    }

    void EmitModulo(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                    const ExpressionBase &right) {
        TypeSpecifier type = BinaryResultType(left.GetType(context), right.GetType(context));
        bool useFloat = type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE;
        bool leftStored = right.ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent(useFloat) : context.AllocateTemporary(useFloat);
        left.EmitRISC(stream, context, leftReg);
        Register rightReg = context.AllocateTemporary(useFloat);
        right.EmitRISC(stream, context, rightReg);

        switch (type) {
            case TypeSpecifier::INT:
                stream << "rem " << result << "," << leftReg << "," << rightReg << std::endl;
                break;
            case TypeSpecifier::CHAR: // todo there is masking here, at least when int is result check!
            case TypeSpecifier::UNSIGNED: // This is after promotion so no need to check signs
                stream << "remu " << result << "," << leftReg << "," << rightReg << std::endl;
                break;
            default:
                throw std::runtime_error("Multiplicative operation attempted on unsupported type.");
        }

        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg);
        context.FreeTemporary(rightReg);
    }

    void EmitAddition(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                      const ExpressionBase &right) {
        TypeSpecifier type = BinaryAdditionResultType(left.GetType(context), right.GetType(context));
        bool leftStored = right.ContainsFunctionCall();
        bool useFloat = type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE;
        Register leftReg = leftStored ? context.AllocatePersistent(useFloat) : context.AllocateTemporary(useFloat);
        left.EmitRISC(stream, context, leftReg);
        Register rightReg = context.AllocateTemporary(useFloat);
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
                int logSize = static_cast<int>(std::log2(type.GetPointeeType().GetTypeSize()));
                if (left.GetType(context).IsPointer()) {
                    // todo wont work for structs
                    // logsize != 0 etc... but can maybe extract this as it will actually be pretty long to stay efficient
                    stream << "slli " << rightReg << "," << rightReg << "," << logSize << std::endl;
                } else {
                    stream << "slli " << leftReg << "," << leftReg << "," << logSize << std::endl;
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
        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg);
        context.FreeTemporary(rightReg);
    }

    void EmitSubtraction(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                         const ExpressionBase &right) {
        TypeSpecifier type = BinarySubtractionResultType(left.GetType(context), right.GetType(context));
        bool leftStored = right.ContainsFunctionCall();
        bool useFloat = type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE;
        Register leftReg = leftStored ? context.AllocatePersistent(useFloat) : context.AllocateTemporary(useFloat);
        left.EmitRISC(stream, context, leftReg);
        Register rightReg = context.AllocateTemporary(useFloat);
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
                int logSize = static_cast<int>(std::log2(left.GetType(context).GetPointeeType().GetTypeSize()));
                // todo wont work for structs
                stream << "sub " << result << "," << leftReg << "," << rightReg << std::endl;
                stream << "srai " << result << "," << result << "," << logSize << std::endl;
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
        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg);
        context.FreeTemporary(rightReg);
    }

    void EmitLeftShift(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                       const ExpressionBase &right) {
        bool leftStored = right.ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent() : context.AllocateTemporary();
        left.EmitRISC(stream, context, leftReg);
        Register rightReg = context.AllocateTemporary();
        right.EmitRISC(stream, context, rightReg);
        stream << "sll " << result << "," << leftReg << "," << rightReg << std::endl;
        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg);
        context.FreeTemporary(rightReg);
    }

    void EmitRightShift(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                        const ExpressionBase &right) {
        bool leftStored = right.ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent() : context.AllocateTemporary();
        left.EmitRISC(stream, context, leftReg);
        Register rightReg = context.AllocateTemporary();
        right.EmitRISC(stream, context, rightReg);
        if (left.GetType(context).IsSigned())
            stream << "sra " << result << "," << leftReg << "," << rightReg << std::endl;
        else
            stream << "srl " << result << "," << leftReg << "," << rightReg << std::endl;
        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg);
        context.FreeTemporary(rightReg);
    }

    void EmitBitwiseAnd(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                        const ExpressionBase &right) {
        bool leftStored = right.ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent() : context.AllocateTemporary();
        left.EmitRISC(stream, context, leftReg);
        Register rightReg = context.AllocateTemporary();
        right.EmitRISC(stream, context, rightReg);
        stream << "and " << result << "," << leftReg << "," << rightReg << std::endl;
        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg);
        context.FreeTemporary(rightReg);
    }

    void EmitBitwiseXor(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                        const ExpressionBase &right) {
        bool leftStored = right.ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent() : context.AllocateTemporary();
        left.EmitRISC(stream, context, leftReg);
        Register rightReg = context.AllocateTemporary();
        right.EmitRISC(stream, context, rightReg);
        stream << "xor " << result << "," << leftReg << "," << rightReg << std::endl;
        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg);
        context.FreeTemporary(rightReg);
    }

    void EmitBitwiseOr(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                       const ExpressionBase &right) {
        bool leftStored = right.ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent() : context.AllocateTemporary();
        left.EmitRISC(stream, context, leftReg);
        Register rightReg = context.AllocateTemporary();
        right.EmitRISC(stream, context, rightReg);
        stream << "or " << result << "," << leftReg << "," << rightReg << std::endl;
        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg);
    }

    TypeSpecifier ResolveTypeAlias(std::vector<TypeSpecifier> specifiers) {
        if (specifiers.size() == 1) {
            return {specifiers.front()};
        }
        std::set<TypeSpecifier::Type> typeSet{specifiers.begin(), specifiers.end()};
        auto it = aliasMap.find(typeSet);
        if (it == aliasMap.end()) {
            throw std::runtime_error("Unsupported type alias");
        }
        return {it->second};
    }

    TypeSpecifier BinaryResultType(const TypeSpecifier &leftType, const TypeSpecifier &rightType) {
        // Choose wider type, and choose unsigned if equal
        if (leftType.GetTypeSize() > rightType.GetTypeSize()) {
            return leftType;
        } else if (leftType.GetTypeSize() == rightType.GetTypeSize()) {
            if (leftType.IsSigned() && !rightType.IsSigned())
                return rightType;
            else
                return leftType;
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

}
