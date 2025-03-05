#include "ast_pointer_declarator.hpp"

namespace ast {

    void PointerDeclarator::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // TODO check but probably nothing to do here.
        // If there is it will just be updating context, which will probably be done higher up in the AST
    }

    void PointerDeclarator::Print(std::ostream &stream) const {
        stream << " *" << GetIdentifier();
    }

    bool PointerDeclarator::IsPointer() const {
        return true;
    }
}
