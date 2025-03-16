#include <cassert>
#include <cmath>
#include "ast_array_index_expression.hpp"

namespace ast {

<<<<<<< HEAD
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
=======
    void ArrayIndexExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // We can safely-ish use destReg as a temporary here as we will always be inside a function
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
                    case TypeSpecifier::UNSIGNED:
                    case TypeSpecifier::INT:
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
                    case TypeSpecifier::POINTER:
                    case TypeSpecifier::VOID:
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
                TypeSpecifier type = context.GetGlobalType(identifier);
                assert(type == TypeSpecifier::POINTER &&
                       "ArrayIndexExpression::EmitRISC() called on a non-pointer/array");
                bool useFloat = type.GetPointeeType() == TypeSpecifier::FLOAT || type.GetPointeeType() == TypeSpecifier::DOUBLE;
                Register indexReg = context.AllocateTemporary();
                index_->EmitRISC(stream, context, indexReg);
                stream << "slli " << indexReg << "," << indexReg << ",2" << std::endl;
                Register addressTemp = useFloat ? context.AllocateTemporary() : destReg;
                stream << "lui " << addressTemp << ", %hi(" << identifier << ")" << std::endl;
                stream << "addi " << addressTemp << "," << addressTemp << ",%lo(" << identifier << ")" << std::endl;
                stream << "add " << addressTemp << "," << addressTemp << "," << indexReg << std::endl;
                switch (GetType(context)) { // Already unwrapped
                    case TypeSpecifier::UNSIGNED:
                    case TypeSpecifier::INT:
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
                    case TypeSpecifier::POINTER:
                    case TypeSpecifier::VOID:

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
>>>>>>> aaabd7d9f74208ee579772452eb7f2b51bfa0e1d
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

<<<<<<< HEAD
    context.FreeTemporary(indexReg);
    return addrReg;
=======
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

>>>>>>> aaabd7d9f74208ee579772452eb7f2b51bfa0e1d
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
