#include "ast_identifier.hpp"

namespace ast {

    void Identifier::EmitRISC(std::ostream &stream, Context &context, int destReg) const {
        // TODO There can be some optimisation if it's already in a reg. but needs some thinking - can we free the source reg?
        auto it = context.CurrentFrame().bindings.find(identifier_);
        if (it == context.CurrentFrame().bindings.end()) {
            throw std::runtime_error("Variable " + identifier_ + " not found in current scope");
        }
        int offset = it->second.offset;

        // Assuming temporary numbering here
        stream << "lw t" << destReg << ", " << offset << "(sp)" << std::endl;
    }

    void Identifier::Print(std::ostream &stream) const {
        stream << identifier_;
    }

    const std::string &Identifier::GetIdentifier() const {
        return identifier_;
    }

}
