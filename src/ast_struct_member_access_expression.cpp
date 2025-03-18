#include "ast_struct_member_access_expression.hpp"

namespace ast {

    void StructMemberAccessExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (context.EmitLHS()) { // Return address
            // Get struct base address
            struct_->EmitRISC(stream, context, destReg);
            // todo ispointer... is that handled below? no not really it seems - need to handle properly, see below branch too

            // Get member offset // todo if we get time, optimise this
            TypeSpecifier structType = struct_->GetType(context);
//            if (structType == TypeSpecifier::POINTER) { // todo Not sure i'm a fan of this, works for hidden pointer PBV on stack params, but doesn't let us use ispointer access properly
//                // it's annoying as well because it is ambiguous for return struct is it meant to be &struct or struct that is a pointer
//                // solution is to copy the struct, which also resolves modifying it illegally when its PBV
//                structType = structType.GetPointeeType();
//                stream << "lw " << destReg << ",0(" << destReg << ")" << std::endl;
//            }
            int offset = 0;
            for (const auto &member: structType.GetStructMembers()) {
                if (member.first == member_) {
                    break;
                }
                offset += member.second.GetTypeSize();
            }
            stream << "addi " << destReg << "," << destReg << "," << offset << std::endl;
        } else { // Return value
            // Get struct base address
            Register addressReg = context.AllocateTemporary();
            bool restore = context.SetEmitLHS(true);
            struct_->EmitRISC(stream, context, addressReg);
            context.SetEmitLHS(restore);
            // Get member offset // todo if we get time, optimise this
            TypeSpecifier structType = struct_->GetType(context);
//            if (structType == TypeSpecifier::POINTER) {
//                structType = structType.GetPointeeType();
//                stream << "lw " << addressReg << ",0(" << addressReg << ")" << std::endl;
//            }
            int offset = 0;
            for (const auto &member: structType.GetStructMembers()) {
                if (member.first == member_) {
                    break;
                }
                offset += member.second.GetTypeSize();
            }
            stream << "addi " << addressReg << "," << addressReg << "," << offset << std::endl;
            TypeSpecifier memberType = GetType(context);
            switch (memberType) {
                case TypeSpecifier::UNSIGNED:
                case TypeSpecifier::INT:
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
                case TypeSpecifier::POINTER:
                case TypeSpecifier::VOID:
                case TypeSpecifier::STRUCT:
                case TypeSpecifier::ARRAY:
                    throw std::runtime_error(
                            "ArrayIndexExpression::EmitRISC() called on an unsupported array type");
            }
            context.FreeTemporary(addressReg);
        }
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
        if (structType == TypeSpecifier::POINTER) {
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
