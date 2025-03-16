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
                case TypeSpecifier::ENUM:
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
                case TypeSpecifier::ARRAY:
                    // Load address
                    stream << "lui " << destReg << ",%hi(" << identifier_ << ")" << std::endl;
                    stream << "addi " << destReg << "," << destReg << ",%lo(" << identifier_ << ")" << std::endl;
                    break;
                case TypeSpecifier::VOID:
                case TypeSpecifier::STRUCT:
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
                case TypeSpecifier::ARRAY:
                    // Load address, but probably never called in local scope
                    stream << "addi " << destReg << ",s0," << offset << std::endl;
                    break;
                case TypeSpecifier::VOID:
                case TypeSpecifier::ENUM:
                case TypeSpecifier::STRUCT:
                    throw std::runtime_error(
                            "Identifier::EmitRISC() called on an unsupported type");
                    // todo handle these
            }
        } else if (context.IsEnum(identifier_)) { // Enumerator identifier
            int value = context.LookupEnum(identifier_);
            stream << "li " << destReg << "," << value << std::endl;
        } else {
            throw std::runtime_error("Identifier::EmitRISC() called on an undeclared identifier");
        }
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
        } else if (context.CurrentFrame().bindings.Contains(identifier_)) {
            Variable var = context.CurrentFrame().bindings.Get(identifier_);
            return var.type;
        } else if (context.IsEnum(identifier_)) {
            return TypeSpecifier{identifier_, false};
        } else {
            // Nice no more sigfaults
            throw std::runtime_error("Identifier::GetType() called on an undeclared identifier");
        }
    }

    bool Identifier::ContainsFunctionCall() const {
        return false;
    }

    std::string Identifier::GetGlobalIdentifier() const {
        return identifier_;
    }

    int Identifier::Evaluate(Context &context) const {
        // Enums can be used here
        if (context.IsEnum(identifier_)) {
            return context.LookupEnum(identifier_);
        }

        throw std::runtime_error("Identifier::Evaluate() called on a non-enum identifier");
    }

    double Identifier::EvaluateFloat(ast::Context &context) const {
        throw std::runtime_error("Identifier::EvaluateFloat() called on an identifier");
    }

}
