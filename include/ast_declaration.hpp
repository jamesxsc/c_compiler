#pragma once

#include "ast_node.hpp"
#include "ast_init_declarator_list.hpp"
#include "ast_type_specifier.hpp"
#include "ast_declaration_specifiers.hpp"

namespace ast {

    class Declaration : public Node {
    public:
        Declaration(DeclarationSpecifiersPtr declarationSpecifiers, InitDeclaratorListPtr initDeclaratorList) :
            declarationSpecifiers_(std::move(declarationSpecifiers)), initDeclaratorList_(std::move(initDeclaratorList)) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;
    private:
        DeclarationSpecifiersPtr declarationSpecifiers_;
        InitDeclaratorListPtr initDeclaratorList_;
    };

    using DeclarationPtr = std::unique_ptr<const Declaration>;

}
