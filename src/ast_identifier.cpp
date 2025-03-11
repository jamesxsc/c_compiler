#include "ast_identifier.hpp"
#include "ast_type_specifier.hpp"
#include "cassert"

namespace ast {

    void Identifier::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // todo check if this is right for structs/arrays etc etc & propagate to to other usages
        TypeSpecifier type = GetType(context);
        if (context.IsGlobal(identifier_)) {
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
                    assert(!IsFloatRegister(destReg) &&
                           "Identifier::EmitRISC attempting to load non-float into float register");
                    stream << "lui " << destReg << ",%hi(" << identifier_ << ")" << std::endl;
                    stream << "lw " << destReg << ",%lo(" << identifier_ << ")(" << destReg << ")" << std::endl;
                    break;
                case TypeSpecifier::CHAR:
                    stream << "lui " << destReg << ",%hi(" << identifier_ << ")" << std::endl;
                    stream << "lbu " << destReg << ",%lo(" << identifier_ << ")(" << destReg << ")" << std::endl;
                    break;
            }
        } else {
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
                    assert(!IsFloatRegister(destReg) &&
                           "Identifier::EmitRISC attempting to load non-float into float register");
                    // todo cx abi - is this actually that bad?
                    stream << "lw " << destReg << "," << offset << "(s0)" << std::endl;
                    break;
                case TypeSpecifier::CHAR:
                    stream << "lbu " << destReg << "," << offset << "(s0)" << std::endl;
                    break;
            }
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

}
