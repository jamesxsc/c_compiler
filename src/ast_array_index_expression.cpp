#include <cassert>
#include <cmath>
#include "ast_array_index_expression.hpp"

namespace ast {

    void ArrayIndexExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        std::string identifier = array_->GetIdentifier();
        if (context.IsArray(identifier)) { // Array syntax
            if (context.IsGlobal(identifier)) {
                Array array = context.GetGlobalArray(identifier);
                bool useFloat = array.type == TypeSpecifier::FLOAT || array.type == TypeSpecifier::DOUBLE;
                assert(useFloat == IsFloatRegister(destReg) &&
                       "ArrayIndexExpression::EmitRISC() destReg array type mismatch");
                Register indexReg = context.AllocateTemporary();
                index_->EmitRISC(stream, context, indexReg);
                int logSize = static_cast<int>(std::log2(GetTypeSize(array.type))); // todo will this break for structs
                if (logSize != 0) // Save an instruction if it's a char array
                    stream << "slli " << indexReg << "," << indexReg << "," << logSize << std::endl;
                Register addressTemp = useFloat ? context.AllocateTemporary() : destReg;
                stream << "lui " << addressTemp << ", %hi(" << identifier << ")" << std::endl;
                stream << "addi " << addressTemp << "," << addressTemp << ",%lo(" << identifier << ")" << std::endl;
                stream << "add " << addressTemp << "," << addressTemp << "," << indexReg << std::endl;
                context.FreeTemporary(indexReg);
                switch (array.type) {
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
                        throw std::runtime_error(
                                "ArrayIndexExpression::EmitRISC() called on a pointer array which is unsupported");
                }
            } else {
                const Variable &variable = context.CurrentFrame().bindings.Get(identifier);
                // Check it is an array
                assert(variable.array && "ArrayIndexExpression::EmitRISC() called on a non-array");
                Array array = static_cast<const Array &>(variable); // NOLINT(*-pro-type-static-cast-downcast)
                bool useFloat = array.type == TypeSpecifier::FLOAT || array.type == TypeSpecifier::DOUBLE;
                assert(useFloat == IsFloatRegister(destReg) &&
                       "ArrayIndexExpression::EmitRISC() destReg array type mismatch");
                Register indexReg = context.AllocateTemporary();
                index_->EmitRISC(stream, context, indexReg);
                int logSize = static_cast<int>(std::log2(GetTypeSize(array.type)));
                if (logSize != 0) // Save an instruction if it's a char array
                    stream << "slli " << indexReg << "," << indexReg << "," << logSize << std::endl;
                Register addressTemp = useFloat ? context.AllocateTemporary() : destReg;
                stream << "addi " << addressTemp << ",s0," << variable.offset << std::endl;
                stream << "add " << addressTemp << "," << addressTemp << "," << indexReg << std::endl;
                context.FreeTemporary(indexReg);
                switch (array.type) {
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
                        throw std::runtime_error(
                                "ArrayIndexExpression::EmitRISC() called on a pointer array which is unsupported");
                }
            }
        } else { // Ptr syntax
            if (context.IsGlobal(identifier)) {
                TypeSpecifier type = context.GetGlobalType(identifier);
                assert(type == TypeSpecifier::POINTER &&
                       "ArrayIndexExpression::EmitRISC() called on a non-pointer/array");
                bool useFloat = IsFloatRegister(
                        destReg); // todo we need to get the underlying type unfortunately to determine what load to use
                // and slli by the correct amount
                Register indexReg = context.AllocateTemporary();
                index_->EmitRISC(stream, context, indexReg);
                stream << "slli " << indexReg << "," << indexReg << ",2" << std::endl;
                Register addressTemp = useFloat ? context.AllocateTemporary() : destReg;
                stream << "lui " << addressTemp << ", %hi(" << identifier << ")" << std::endl;
                stream << "addi " << addressTemp << "," << addressTemp << ",%lo(" << identifier << ")" << std::endl;
                stream << "add " << addressTemp << "," << addressTemp << "," << indexReg << std::endl;
                stream << "lw " << destReg << ",0" << "(" << addressTemp << ")" << std::endl; // here ^
                context.FreeTemporary(indexReg);
            } else {
                Variable variable = context.CurrentFrame().bindings.Get(identifier);
                assert(variable.type == TypeSpecifier::POINTER &&
                       "ArrayIndexExpression::EmitRISC() called on a non-pointer/array");
                bool useFloat = IsFloatRegister(destReg); // todo see above
                Register indexReg = context.AllocateTemporary();
                index_->EmitRISC(stream, context, indexReg);
                stream << "slli " << indexReg << "," << indexReg << ",2" << std::endl;
                Register addressTemp = useFloat ? context.AllocateTemporary() : destReg;
                stream << "lw " << addressTemp << "," << variable.offset << "(s0)" << std::endl;
                stream << "add " << addressTemp << "," << addressTemp << "," << indexReg << std::endl;
                stream << "lw " << destReg << ",0(" << addressTemp << ")" << std::endl;
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
        return array_->GetType(
                context); // todo where do we differentiate type of array and elements? cx; ok for the moment, type of pointer is a bigger problem
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
