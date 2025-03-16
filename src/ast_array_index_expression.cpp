#include <cassert>
#include <cmath>
#include "ast_array_index_expression.hpp"

namespace ast {

Register ArrayIndexExpression::EmitAddressRISC(std::ostream &stream, Context &context) const
{
    std::string identifier = array_->GetIdentifier();
    Register addrReg = context.AllocateTemporary();
    Register indexReg = context.AllocateTemporary();
    index_->EmitRISC(stream, context, indexReg);

    if (context.IsArray(identifier)) {
        if (context.IsGlobal(identifier)) {
            TypeSpecifier array = context.GetGlobalType(identifier);
            int logSize = static_cast<int>(std::log2(array.GetArrayType().GetTypeSize()));
            if (logSize != 0) {
                stream << "slli " << indexReg << "," << indexReg << "," << logSize << std::endl;
            }
            stream << "lui " << addrReg << ", %hi(" << identifier << ")" << std::endl;
            stream << "addi " << addrReg << "," << addrReg << ",%lo(" << identifier << ")" << std::endl;
            stream << "add " << addrReg << "," << addrReg << "," << indexReg << std::endl;
        } else {
            const Variable &variable = context.CurrentFrame().bindings.Get(identifier);
            TypeSpecifier arrayType = variable.type.GetArrayType();
            int logSize = static_cast<int>(std::log2(arrayType.GetTypeSize()));
            if (logSize != 0) {
                stream << "slli " << indexReg << "," << indexReg << "," << logSize << std::endl;
            }
            stream << "addi " << addrReg << ",s0," << variable.offset << std::endl;
            stream << "add " << addrReg << "," << addrReg << "," << indexReg << std::endl;
        }
    } else {
        if (context.IsGlobal(identifier)) {
            TypeSpecifier type = context.GetGlobalType(identifier);
            stream << "slli " << indexReg << "," << indexReg << ",2" << std::endl;
            stream << "lui " << addrReg << ", %hi(" << identifier << ")" << std::endl;
            stream << "addi " << addrReg << "," << addrReg << ",%lo(" << identifier << ")" << std::endl;
            stream << "add " << addrReg << "," << addrReg << "," << indexReg << std::endl;
        } else {
            Variable variable = context.CurrentFrame().bindings.Get(identifier);
            stream << "slli " << indexReg << "," << indexReg << ",2" << std::endl;
            stream << "lw " << addrReg << "," << variable.offset << "(s0)" << std::endl;
            stream << "add " << addrReg << "," << addrReg << "," << indexReg << std::endl;
        }
    }

    context.FreeTemporary(indexReg);
    return addrReg;
}

void ArrayIndexExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
    std::string identifier = array_->GetIdentifier();
    if (context.IsArray(identifier)) {
        if (context.IsGlobal(identifier)) {
            TypeSpecifier array = context.GetGlobalType(identifier);
            bool useFloat = array.GetArrayType() == TypeSpecifier::FLOAT || array.GetArrayType() == TypeSpecifier::DOUBLE;
            assert(useFloat == IsFloatRegister(destReg));
            Register indexReg = context.AllocateTemporary();
            index_->EmitRISC(stream, context, indexReg);
            int logSize = static_cast<int>(std::log2(array.GetArrayType().GetTypeSize()));
            if (logSize != 0)
                stream << "slli " << indexReg << "," << indexReg << "," << logSize << std::endl;
            Register addressTemp = useFloat ? context.AllocateTemporary() : destReg;
            stream << "lui " << addressTemp << ", %hi(" << identifier << ")" << std::endl;
            stream << "addi " << addressTemp << "," << addressTemp << ",%lo(" << identifier << ")" << std::endl;
            stream << "add " << addressTemp << "," << addressTemp << "," << indexReg << std::endl;
            context.FreeTemporary(indexReg);
            switch (array.GetArrayType()) {
                case TypeSpecifier::UNSIGNED:
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
                default:
                    throw std::runtime_error("ArrayIndexExpression::EmitRISC() unsupported array type");
            }
        } else {
            const Variable &variable = context.CurrentFrame().bindings.Get(identifier);
            assert(variable.type.IsArray());
            TypeSpecifier arrayType = variable.type.GetArrayType();
            bool useFloat = arrayType == TypeSpecifier::FLOAT || arrayType == TypeSpecifier::DOUBLE;
            assert(useFloat == IsFloatRegister(destReg));
            Register indexReg = context.AllocateTemporary();
            index_->EmitRISC(stream, context, indexReg);
            int logSize = static_cast<int>(std::log2(arrayType.GetTypeSize()));
            if (logSize != 0)
                stream << "slli " << indexReg << "," << indexReg << "," << logSize << std::endl;
            Register addressTemp = useFloat ? context.AllocateTemporary() : destReg;
            stream << "addi " << addressTemp << ",s0," << variable.offset << std::endl;
            stream << "add " << addressTemp << "," << addressTemp << "," << indexReg << std::endl;
            context.FreeTemporary(indexReg);
            switch (arrayType) {
                case TypeSpecifier::UNSIGNED:
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
                default:
                    throw std::runtime_error("ArrayIndexExpression::EmitRISC() unsupported array type");
            }
        }
    } else {
        if (context.IsGlobal(identifier)) {
            TypeSpecifier type = context.GetGlobalType(identifier);
            assert(type == TypeSpecifier::POINTER);
            bool useFloat = type.GetPointeeType() == TypeSpecifier::FLOAT || type.GetPointeeType() == TypeSpecifier::DOUBLE;
            Register indexReg = context.AllocateTemporary();
            index_->EmitRISC(stream, context, indexReg);
            stream << "slli " << indexReg << "," << indexReg << ",2" << std::endl;
            Register addressTemp = useFloat ? context.AllocateTemporary() : destReg;
            stream << "lui " << addressTemp << ", %hi(" << identifier << ")" << std::endl;
            stream << "addi " << addressTemp << "," << addressTemp << ",%lo(" << identifier << ")" << std::endl;
            stream << "add " << addressTemp << "," << addressTemp << "," << indexReg << std::endl;
            switch (GetType(context)) {
                case TypeSpecifier::UNSIGNED:
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
                default:
                    throw std::runtime_error("ArrayIndexExpression::EmitRISC() unsupported pointer type");
            }
            context.FreeTemporary(indexReg);
        } else {
            Variable variable = context.CurrentFrame().bindings.Get(identifier);
            assert(variable.type == TypeSpecifier::POINTER);
            bool useFloat = variable.type.GetPointeeType() == TypeSpecifier::FLOAT || variable.type.GetPointeeType() == TypeSpecifier::DOUBLE;
            Register indexReg = context.AllocateTemporary();
            index_->EmitRISC(stream, context, indexReg);
            stream << "slli " << indexReg << "," << indexReg << ",2" << std::endl;
            Register addressTemp = useFloat ? context.AllocateTemporary() : destReg;
            stream << "lw " << addressTemp << "," << variable.offset << "(s0)" << std::endl;
            stream << "add " << addressTemp << "," << addressTemp << "," << indexReg << std::endl;
            switch (GetType(context)) {
                case TypeSpecifier::UNSIGNED:
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
                default:
                    throw std::runtime_error("ArrayIndexExpression::EmitRISC() unsupported pointer type");
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
    throw std::runtime_error("ArrayIndexExpression::GetGlobalValue() not constant");
}

std::string ArrayIndexExpression::GetIdentifier() const {
    return array_->GetIdentifier();
}

const Expression &ArrayIndexExpression::GetIndexExpression() const {
    return *index_;
}

int ArrayIndexExpression::Evaluate() const {
    throw std::runtime_error("ArrayIndexExpression::Evaluate() not constant");
}

} // namespace ast
