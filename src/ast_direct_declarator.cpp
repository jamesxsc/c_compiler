#include "ast_direct_declarator.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void DirectDeclarator::Print(std::ostream &stream) const {

    }

    void DirectDeclarator::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {

    }

    const std::string &DirectDeclarator::GetIdentifier() const {
        return identifier_;
    }



    ast::Type DirectDeclarator::GetType(Context&) const {
        return ast::Type(ast::TypeSpecifier::INT, true);
    }
}