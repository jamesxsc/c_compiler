#include "ast_identifier.hpp"
#include "ast_type_specifier.hpp"
#include "cassert"

namespace ast {

    void Identifier::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Variable identifier
        if (context.IsGlobal(identifier_)) {
            TypeSpecifier type = GetType(context);
            switch (type) {
                case TypeSpecifier::DOUBLE:
                case TypeSpecifier::FLOAT: {
                    assert(IsFloatRegister(destReg) &&
                           "Identifier::EmitRISC attempting to load float into integer register");
                    Register tempIntReg = context.AllocateTemporary();
                    stream << "lui " << tempIntReg << ",%hi(" << identifier_ << ")" << std::endl;
                    stream << (type == TypeSpecifier::FLOAT ? "flw " : "fld ") << destReg << ",%lo(" << identifier_
                           << ")(" << tempIntReg << ")" << std::endl;
                    context.FreeTemporary(tempIntReg);
                    break;
                }
                case TypeSpecifier::POINTER:
                case TypeSpecifier::INT:
                case TypeSpecifier::UNSIGNED:
                    assert(!IsFloatRegister(destReg) &&
                           "Identifier::EmitRISC attempting to load non-float into float register");
                    stream << "lui " << destReg << ",%hi(" << identifier_ << ")" << std::endl;
                    stream << "lw " << destReg << ",%lo(" << identifier_ << ")(" << destReg << ")" << std::endl;
                    break;
                case TypeSpecifier::CHAR:
                    stream << "lui " << destReg << ",%hi(" << identifier_ << ")" << std::endl;
                    stream << "lbu " << destReg << ",%lo(" << identifier_ << ")(" << destReg << ")" << std::endl;
                    break;
                case TypeSpecifier::VOID:
                case TypeSpecifier::ENUM:
                case TypeSpecifier::STRUCT:
                case TypeSpecifier::ARRAY: // Unsupported since it is handled in ArrayIndexExpression
                    throw std::runtime_error(
                            "Identifier::EmitRISC() called on an unsupported type");
                    // todo handle these
            }
        } else if (context.CurrentFrame().bindings.Contains(identifier_)) {
            TypeSpecifier type = GetType(context);
            int offset = context.CurrentFrame().bindings.Get(identifier_).offset;
            switch (type) {
                case TypeSpecifier::FLOAT:
                case TypeSpecifier::DOUBLE:
                    assert(IsFloatRegister(destReg) &&
                           "Identifier::EmitRISC attempting to load double into integer register");
                    stream << (type == TypeSpecifier::FLOAT ? "flw " : "fld ") << destReg << "," << offset << "(s0)"
                           << std::endl;
                    break;
                case TypeSpecifier::POINTER:
                case TypeSpecifier::INT:
                case TypeSpecifier::UNSIGNED:
                    assert(!IsFloatRegister(destReg) &&
                           "Identifier::EmitRISC attempting to load non-float into float register");
                    stream << "lw " << destReg << "," << offset << "(s0)" << std::endl;
                    break;
                case TypeSpecifier::CHAR:
                    stream << "lbu " << destReg << "," << offset << "(s0)" << std::endl;
                    break;
                case TypeSpecifier::VOID:
                case TypeSpecifier::ENUM:
                case TypeSpecifier::STRUCT:
                case TypeSpecifier::ARRAY: // Unsupported since it is handled in ArrayIndexExpression
                    throw std::runtime_error(
                            "Identifier::EmitRISC() called on an unsupported type");
                    // todo handle these
            }
        }
        // Enumerator identifier

    }

    void Identifier::Print(std::ostream &stream) const {
        stream << identifier_;
    }

    const std::string &Identifier::GetIdentifier() const {
        return identifier_;
    }

    TypeSpecifier Identifier::GetType(Context &context) const {
        if (context.IsGlobal(identifier_)) {
            return context.GetGlobalType(identifier_);
        } else {
            Variable var = context.CurrentFrame().bindings.Get(identifier_);
            return var.type;
        }
    }

    bool Identifier::ContainsFunctionCall() const {
        return false;
    }

    std::string Identifier::GetGlobalIdentifier() const {
        return identifier_;
    }

    int Identifier::GetGlobalValue() const {
        throw std::runtime_error("Identifier::GetGlobalValue() called on a identifier");
    }

    int Identifier::Evaluate() const {
        throw std::runtime_error("Identifier::Evaluate() called on a identifier");
    }

}
