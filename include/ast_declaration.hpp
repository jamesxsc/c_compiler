#pragma once

#include "ast_node.hpp"
#include "ast_init_declarator_list.hpp"
#include "ast_type_specifier.hpp"
#include "ast_declaration_specifiers.hpp"
#include "ast_initializer_list.hpp"

namespace ast {

    class Declaration;
    using DeclarationPtr = std::unique_ptr<Declaration>;
    class Declaration : public Node {
    public:
        Declaration(DeclarationSpecifiersPtr declarationSpecifiers, InitDeclaratorListPtr initDeclaratorList) :
            declarationSpecifiers_(std::move(declarationSpecifiers)), initDeclaratorList_(std::move(initDeclaratorList)) {}

        explicit Declaration(DeclarationPtr other): declarationSpecifiers_(std::move(other->declarationSpecifiers_)), initDeclaratorList_(std::move(other->initDeclaratorList_)) {}

        virtual void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;

        [[nodiscard]] bool IsTypedef() const;
        [[nodiscard]] virtual bool IsAggregateTypeDefinition() const;

    protected:
        DeclarationSpecifiersPtr declarationSpecifiers_;
        InitDeclaratorListPtr initDeclaratorList_;

        static void
        EmitStructInitializer(const InitializerList &initializerList, const TypeSpecifier &type, Register &tempReg,
                              int baseOffset, std::ostream &stream, Context &context) ;
    };

}
