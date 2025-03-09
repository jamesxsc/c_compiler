#include "ast_pointer_declarator.hpp"

namespace ast {

    void PointerDeclarator::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
    }

    void PointerDeclarator::Print(std::ostream &stream) const {
        stream << "*" << GetIdentifier();
    }

    bool PointerDeclarator::IsPointer() const {
        return true;
    }
}
