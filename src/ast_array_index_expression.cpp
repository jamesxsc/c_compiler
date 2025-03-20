#include <cassert>
#include <cmath>
#include "ast_array_index_expression.hpp"
#include "risc_utils.hpp"

namespace ast {

    void ArrayIndexExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (context.EmitLHS()) {
            Register indexReg = context.AllocateTemporary(stream);

            TypeSpecifier type = array_->GetType(context);
            bool isPointerSyntax = type.IsPointer();
            TypeSpecifier arrayType = isPointerSyntax ? type.GetPointeeType() : type.GetArrayType();

            { Context::ScopedEmitLHS guard(context, false); // Emit/evaluate this normally
                index_->EmitRISC(stream, context, indexReg);
                Utils::EmitIndexToAddressOffset(stream, indexReg, context, arrayType);
            }

            // Need LHS here, get base address of array
            { Context::ScopedEmitLHS guard(context, !isPointerSyntax);
                array_->EmitRISC(stream, context, destReg);
                stream << "add " << destReg << "," << destReg << "," << indexReg << std::endl;
            }

            context.FreeTemporary(indexReg, stream);
            return;
        }

        // global/local is handled down in identifier, only have to split on ptr here
        TypeSpecifier type = array_->GetType(context);
        bool isPointerSyntax = type.IsPointer();

        TypeSpecifier arrayType = isPointerSyntax ? type.GetPointeeType() : type.GetArrayType();
        bool useFloat = arrayType == TypeSpecifier::FLOAT || arrayType == TypeSpecifier::DOUBLE;
        Register addressTemp = useFloat ? context.AllocateTemporary(stream) : destReg; // todo here and elsewhere, do a sweep for clobbering potential
        { Context::ScopedEmitLHS guard(context, !isPointerSyntax);
            array_->EmitRISC(stream, context, addressTemp);
        }
        Register indexReg = context.AllocateTemporary(stream);
        index_->EmitRISC(stream, context, indexReg);
        Utils::EmitIndexToAddressOffset(stream, indexReg, context, arrayType);
        stream << "add " << addressTemp << "," << addressTemp << "," << indexReg << std::endl;
        context.FreeTemporary(indexReg, stream);
        EmitLoadElement(arrayType, stream, context, destReg, addressTemp);

        assert(useFloat == IsFloatRegister(destReg) &&
               "ArrayIndexExpression::EmitRISC() destReg array type mismatch");
        if (useFloat)
            context.FreeTemporary(addressTemp, stream);
    }

    void ArrayIndexExpression::EmitLoadElement(const TypeSpecifier &elementType, std::ostream &stream, Context &context,
                                               const Register &destReg, const Register &addressTemp) {
        switch (elementType) {
            case TypeSpecifier::UNSIGNED:
            case TypeSpecifier::INT:
            case TypeSpecifier::POINTER:
            case TypeSpecifier::ENUM:
                stream << "lw " << destReg << ",0(" << addressTemp << ")" << std::endl;
                break;
            case TypeSpecifier::FLOAT:
                stream << "flw " << destReg << ",0(" << addressTemp << ")" << std::endl;
                break;
            case TypeSpecifier::DOUBLE:
                stream << "fld " << destReg << ",0(" << addressTemp << ")" << std::endl;
                break;
            case TypeSpecifier::CHAR:
                stream << "lbu " << destReg << ",0(" << addressTemp << ")" << std::endl;
                break;
            case TypeSpecifier::VOID:
            case TypeSpecifier::STRUCT:
            case TypeSpecifier::ARRAY:
                throw std::runtime_error(
                        "ArrayIndexExpression::EmitRISC() called on an unsupported array type");
        }
    }

    void ArrayIndexExpression::Print(std::ostream &stream) const {
        array_->Print(stream);
        stream << "[";
        index_->Print(stream);
        stream << "]";
    }

    bool ArrayIndexExpression::ContainsFunctionCall() const {
        return index_->ContainsFunctionCall();
    }

    TypeSpecifier ArrayIndexExpression::GetType(Context &context) const {
        const TypeSpecifier &childType = array_->GetType(context);
        if (childType.IsPointer()) {
            return childType.GetPointeeType();
        } else {
            assert(childType.IsArray() && "ArrayIndexExpression::GetType() called on a non-pointer/array");
            return childType.GetArrayType();
        }
    }

    std::string ArrayIndexExpression::GetGlobalIdentifier() const {
        return array_->GetGlobalIdentifier();
    }

    std::string ArrayIndexExpression::GetIdentifier() const {
        return array_->GetIdentifier();
    }

    const Expression &ArrayIndexExpression::GetIndexExpression() const {
        return *index_;
    }

    int ArrayIndexExpression::Evaluate(Context &context) const {
        throw std::runtime_error("ArrayIndexExpression::Evaluate() called on an array index expression");
    }

    double ArrayIndexExpression::EvaluateFloat(ast::Context &context) const {
        throw std::runtime_error("ArrayIndexExpression::EvaluateFloat() called on an array index expression");
    }

}
