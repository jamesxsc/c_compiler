#pragma once

#include "ast_node.hpp"

namespace ast {

    class SpecifierQualifierList : public Node {
    public:
        explicit SpecifierQualifierList(TypeSpecifier first) : specifiers_() {
            specifiers_.push_back(first);
        }

        void AddTypeSpecifier(TypeSpecifier specifier);

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;
        TypeSpecifier GetType(Context &context) const;

    private:
        std::vector<TypeSpecifier> specifiers_;
    };
    using SpecifierQualifierListPtr = std::unique_ptr<const SpecifierQualifierList>;

}
