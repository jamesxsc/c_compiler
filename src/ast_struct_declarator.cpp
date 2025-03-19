#include "ast_struct_declarator.hpp"

namespace ast {

    void StructDeclarator::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {

    }

    void StructDeclarator::Print(std::ostream &stream) const {
        stream << GetIdentifier();
    }

}
