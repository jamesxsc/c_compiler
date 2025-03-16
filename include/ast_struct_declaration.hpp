#pragma once

#include "ast_node.hpp"
#include "ast_struct_declarator_list.hpp"
#include "ast_specifier_qualifier_list.hpp"

namespace ast {

    class StructDeclaration : public Node {
    public:
        StructDeclaration(SpecifierQualifierListPtr specifierQualifierList,
                          StructDeclaratorListPtr structDeclaratorList)
                : specifierQualifierList_(std::move(specifierQualifierList)),
                  structDeclaratorList_(std::move(structDeclaratorList)) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] TypeSpecifier GetType(Context &context) const;

        [[nodiscard]] const std::vector<StructDeclaratorPtr> &GetDeclarators() const;

    private:
        SpecifierQualifierListPtr specifierQualifierList_;
        StructDeclaratorListPtr structDeclaratorList_;
    };

    using StructDeclarationPtr = std::unique_ptr<StructDeclaration>;

}
