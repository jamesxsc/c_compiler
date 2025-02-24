#include "ast_direct_declarator.hpp"

namespace ast {

    void DirectDeclarator::Print(std::ostream &stream) const {

    }

    void DirectDeclarator::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {

    }

    const std::string &DirectDeclarator::GetIdentifier() const {
        return identifier_;
    }

}
