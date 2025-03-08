#include "ast_identifier.hpp"
#include "ast_type_specifier.hpp"
#include "cassert"

namespace ast {

    void Identifier::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        int offset = context.CurrentFrame().bindings.Get(identifier_).offset;

        stream << "lw " << destReg << "," << offset << "(s0)" << std::endl;
    }

    void Identifier::Print(std::ostream &stream) const {
        stream << identifier_;
    }

    const std::string &Identifier::GetIdentifier() const {
        return identifier_;
    }

    TypeSpecifier Identifier::GetType(Context& context) const {
        Variable var = context.CurrentFrame().bindings.Get(identifier_);
        return var.type;
    }

    bool Identifier::ContainsFunctionCall() const {
        return false;
    }

}
