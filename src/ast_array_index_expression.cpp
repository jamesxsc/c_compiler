#include <cassert>
#include <cmath>
#include "ast_array_index_expression.hpp"

namespace ast {

    void ArrayIndexExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        std::string identifier = array_->GetIdentifier();
        if (context.IsArray(identifier)) { // Array syntax
            if (context.IsGlobal(identifier)) {
                TypeSpecifier array = context.GetGlobalType(identifier);
                bool useFloat = array.GetArrayType() == TypeSpecifier::FLOAT || array.GetArrayType() == TypeSpecifier::DOUBLE;
                assert(useFloat == IsFloatRegister(destReg) &&
                       "ArrayIndexExpression::EmitRISC() destReg array type mismatch");
                Register indexReg = context.AllocateTemporary();
                index_->EmitRISC(stream, context, indexReg);
                int logSize = static_cast<int>(std::log2(array.GetArrayType().GetTypeSize())); // todo will this break for structs
                if (logSize != 0) // Save an instruction if it's a char array
                    stream << "slli " << indexReg << "," << indexReg << "," << logSize << std::endl;
                Register addressTemp = useFloat ? context.AllocateTemporary() : destReg;
                stream << "lui " << addressTemp << ", %hi(" << identifier << ")" << std::endl;
                stream << "addi " << addressTemp << "," << addressTemp << ",%lo(" << identifier << ")" << std::endl;
                stream << "add " << addressTemp << "," << addressTemp << "," << indexReg << std::endl;
                context.FreeTemporary(indexReg);
                switch (array.GetArrayType()) {
                    case TypeSpecifier::INT:
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
                    case TypeSpecifier::POINTER:
                    case TypeSpecifier::VOID:
                    case TypeSpecifier::ENUM:
                    case TypeSpecifier::STRUCT:
                    case TypeSpecifier::ARRAY:
                        throw std::runtime_error(
                                "ArrayIndexExpression::EmitRISC() called on an unsupported array type");
                }
            } else {
                const Variable &variable = context.CurrentFrame().bindings.Get(identifier);
                // Check it is an array
                assert(variable.type.IsArray() && "ArrayIndexExpression::EmitRISC() called on a non-array");
                TypeSpecifier arrayType = variable.type.GetArrayType();
                bool useFloat = arrayType == TypeSpecifier::FLOAT || arrayType == TypeSpecifier::DOUBLE;
                assert(useFloat == IsFloatRegister(destReg) &&
                       "ArrayIndexExpression::EmitRISC() destReg array type mismatch");
                Register indexReg = context.AllocateTemporary();
                index_->EmitRISC(stream, context, indexReg);
                int logSize = static_cast<int>(std::log2(arrayType.GetTypeSize()));
                if (logSize != 0) // Save an instruction if it's a char array
                    stream << "slli " << indexReg << "," << indexReg << "," << logSize << std::endl;
                Register addressTemp = useFloat ? context.AllocateTemporary() : destReg;
                stream << "addi " << addressTemp << ",s0," << variable.offset << std::endl;
                stream << "add " << addressTemp << "," << addressTemp << "," << indexReg << std::endl;
                context.FreeTemporary(indexReg);
                switch (arrayType) {
                    case TypeSpecifier::INT:
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
                    case TypeSpecifier::POINTER:
                    case TypeSpecifier::VOID:
                    case TypeSpecifier::ENUM:
                    case TypeSpecifier::STRUCT:
                    case TypeSpecifier::ARRAY:
                        throw std::runtime_error(
                                "ArrayIndexExpression::EmitRISC() called on an unsupported array type");
                        // todo handle these
                }
            }
        } else { // Ptr syntax
            if (context.IsGlobal(identifier)) {
                // todo cx classes like this manual assertion for destreg usability?
                TypeSpecifier type = context.GetGlobalType(identifier);
                assert(type == TypeSpecifier::POINTER &&
                       "ArrayIndexExpression::EmitRISC() called on a non-pointer/array");
                // todo double cx that equality converts and equates correctly
                bool useFloat = type.GetPointeeType() == TypeSpecifier::FLOAT || type.GetPointeeType() == TypeSpecifier::DOUBLE;
                // and slli by the correct amount
                Register indexReg = context.AllocateTemporary();
                index_->EmitRISC(stream, context, indexReg);
                stream << "slli " << indexReg << "," << indexReg << ",2" << std::endl;
                Register addressTemp = useFloat ? context.AllocateTemporary() : destReg;
                stream << "lui " << addressTemp << ", %hi(" << identifier << ")" << std::endl;
                stream << "addi " << addressTemp << "," << addressTemp << ",%lo(" << identifier << ")" << std::endl;
                stream << "add " << addressTemp << "," << addressTemp << "," << indexReg << std::endl;
                switch (GetType(context)) { // Already unwrapped
                    case TypeSpecifier::INT:
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
                    case TypeSpecifier::POINTER:
                    case TypeSpecifier::VOID:
                    case TypeSpecifier::ENUM:
                    case TypeSpecifier::STRUCT:
                    case TypeSpecifier::ARRAY:
                        throw std::runtime_error(
                                "ArrayIndexExpression::EmitRISC() called on an unsupported array type");
                        // todo handle these
                }
                stream << "lw " << destReg << ",0" << "(" << addressTemp << ")" << std::endl; // here ^
                context.FreeTemporary(indexReg);
            } else {
                Variable variable = context.CurrentFrame().bindings.Get(identifier);
                assert(variable.type == TypeSpecifier::POINTER &&
                       "ArrayIndexExpression::EmitRISC() called on a non-pointer/array");
                bool useFloat = variable.type.GetPointeeType() == TypeSpecifier::FLOAT || variable.type.GetPointeeType() == TypeSpecifier::DOUBLE;
                Register indexReg = context.AllocateTemporary();
                index_->EmitRISC(stream, context, indexReg);
                stream << "slli " << indexReg << "," << indexReg << ",2" << std::endl;
                Register addressTemp = useFloat ? context.AllocateTemporary() : destReg;
                stream << "lw " << addressTemp << "," << variable.offset << "(s0)" << std::endl;
                stream << "add " << addressTemp << "," << addressTemp << "," << indexReg << std::endl;
                switch (GetType(context)) { // Already unwrapped
                    case TypeSpecifier::INT:
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
                    case TypeSpecifier::POINTER:
                    case TypeSpecifier::VOID:
                    case TypeSpecifier::ENUM:
                    case TypeSpecifier::STRUCT:
                    case TypeSpecifier::ARRAY:
                        throw std::runtime_error(
                                "ArrayIndexExpression::EmitRISC() called on an unsupported array type");
                        // todo handle these
                }
                context.FreeTemporary(indexReg);
            }
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
        if (context.IsArray(array_->GetIdentifier())) {
            return array_->GetType(context).GetArrayType();
        } else {
            return array_->GetType(context).GetPointeeType();
        }
    }

    std::string ArrayIndexExpression::GetGlobalIdentifier() const {
        return array_->GetGlobalIdentifier();
    }

    int ArrayIndexExpression::GetGlobalValue() const {
        // probably do need to implement this and the above
        throw std::runtime_error("ArrayIndexExpression::GetGlobalValue() called on an array index expression");
    }

    std::string ArrayIndexExpression::GetIdentifier() const {
        return array_->GetIdentifier();
    }

    const Expression &ArrayIndexExpression::GetIndexExpression() const {
        return *index_;
    }

}
