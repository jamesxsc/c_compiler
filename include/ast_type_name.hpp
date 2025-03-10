#pragma once

#include "ast_node.hpp"
#include "ast_specifier_qualifier_list.hpp"

namespace ast {

    class TypeName : public Node {
    public:
        explicit TypeName(SpecifierQualifierListPtr specifierQualifierList) : specifierQualifierList_(std::move(specifierQualifierList)) {};

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;
        TypeSpecifier GetType(Context &context) const;

    private:
        SpecifierQualifierListPtr specifierQualifierList_;
    };
    using TypeNamePtr = std::unique_ptr<const TypeName>;

}
