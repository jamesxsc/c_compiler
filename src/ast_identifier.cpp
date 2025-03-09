#include "ast_identifier.hpp"
#include "ast_type_specifier.hpp"
#include "cassert"

namespace ast {

    void Identifier::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (context.IsGlobal(identifier_)) {
            // todo check if this is right for floats/doubles/structs/arrays etc etc & propagate to to other usages
            stream << "lui " << destReg << ",%hi(" << identifier_ << ")" << std::endl;
            stream << "lw " << destReg << ",%lo(" << identifier_ << ")(" << destReg << ")" << std::endl;
        } else {
            int offset = context.CurrentFrame().bindings.Get(identifier_).offset;
            stream << "lw " << destReg << "," << offset << "(s0)" << std::endl;
        }
    }

    void Identifier::Print(std::ostream &stream) const {
        stream << identifier_;
    }

    const std::string &Identifier::GetIdentifier() const {
        return identifier_;
    }

    TypeSpecifier Identifier::GetType(Context &context) const {
        Variable var = context.CurrentFrame().bindings.Get(identifier_);
        return var.type;
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
