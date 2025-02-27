#pragma once

#include "ast_node.hpp"
#include "ast_init_declarator_list.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    class Declaration : public Node {
    public:
        Declaration(TypeSpecifier typeSpecifier, InitDeclaratorListPtr initDeclaratorList) :
            typeSpecifier_(typeSpecifier), initDeclaratorList_(std::move(initDeclaratorList)) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;
    private:
        // TODO actual specifiers structure
        TypeSpecifier typeSpecifier_;
        InitDeclaratorListPtr initDeclaratorList_;
    };

    using DeclarationPtr = std::unique_ptr<const Declaration>;

}
