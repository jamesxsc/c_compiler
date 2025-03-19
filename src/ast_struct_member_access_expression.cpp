#include "ast_struct_member_access_expression.hpp"

namespace ast {

    void StructMemberAccessExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Get struct base address
        Register addressReg = context.AllocateTemporary();
        bool restore = context.SetEmitLHS(true);
        struct_->EmitRISC(stream, context, addressReg);
        context.SetEmitLHS(restore);
        // Get member offset
        TypeSpecifier structType = struct_->GetType(context);
        if (pointerAccess_) {
            assert(structType == TypeSpecifier::POINTER && "Pointer access on non-pointer");
            structType = structType.GetPointeeType();
            stream << "lw " << destReg << ",0(" << destReg << ")" << std::endl; // Load address stored in ptr
        }

        int offset = structType.GetStructMemberOffset(member_);
        stream << "addi " << addressReg << "," << addressReg << "," << offset << std::endl;

        if (context.EmitLHS()) { // Just return address
            stream << "mv " << destReg << "," << addressReg << std::endl;
            context.FreeTemporary(addressReg);
            return;
        }

        TypeSpecifier memberType = GetType(context);
        switch (memberType) {
            case TypeSpecifier::UNSIGNED:
            case TypeSpecifier::INT:
            case TypeSpecifier::POINTER:
            case TypeSpecifier::ENUM:
                stream << "lw " << destReg << ",0(" << addressReg << ")" << std::endl;
                break;
            case TypeSpecifier::FLOAT:
                stream << "flw " << destReg << ",0(" << addressReg << ")" << std::endl;
                break;
            case TypeSpecifier::DOUBLE:
                stream << "fld " << destReg << ",0(" << addressReg << ")" << std::endl;
                break;
            case TypeSpecifier::CHAR:
                stream << "lbu " << destReg << ",0(" << addressReg << ")" << std::endl;
                break;
            case TypeSpecifier::VOID:
            case TypeSpecifier::STRUCT:
            case TypeSpecifier::ARRAY:
                throw std::runtime_error(
                        "ArrayIndexExpression::EmitRISC() called on an unsupported array type");
        }
        context.FreeTemporary(addressReg);
    }

    void StructMemberAccessExpression::Print(std::ostream &stream) const {
        struct_->Print(stream);
        if (pointerAccess_) {
            stream << "->" << member_;
        } else {
            stream << "." << member_;
        }
    }

    TypeSpecifier StructMemberAccessExpression::GetType(Context &context) const {
        TypeSpecifier structType = struct_->GetType(context);
        if (pointerAccess_) {
            assert(structType == TypeSpecifier::POINTER && "Pointer access on non-pointer");
            structType = structType.GetPointeeType();
        }

        if (structType != TypeSpecifier::Type::STRUCT) {
            throw std::runtime_error("StructMemberAccessExpression::GetType() called on a non-struct");
        }
        for (const auto &member: structType.GetStructMembers()) {
            if (member.first == member_) {
                return member.second;
            }
        }
        throw std::runtime_error("StructMemberAccessExpression::GetType() called on a non-existent member");
    }

    double StructMemberAccessExpression::EvaluateFloat(Context &context) const {
        throw std::runtime_error("StructMemberAccessExpression::EvaluateFloat() called on a non-constant");
    }

    int StructMemberAccessExpression::Evaluate(Context &context) const {
        throw std::runtime_error("StructMemberAccessExpression::Evaluate() called on a non-constant");
    }

    bool StructMemberAccessExpression::ContainsFunctionCall() const {
        return false;
    }

    std::string StructMemberAccessExpression::GetGlobalIdentifier() const {
        throw std::runtime_error("StructMemberAccessExpression::GetGlobalIdentifier() called on a non &'d global");
    }
}
