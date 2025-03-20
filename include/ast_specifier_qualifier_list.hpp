#pragma once

#include "ast_node.hpp"

namespace ast {

    class SpecifierQualifierList : public Node {
    public:
        explicit SpecifierQualifierList(TypeSpecifierPtr first) : specifiers_() {
            specifiers_.push_back(*std::move(first));
        }

        void AddTypeSpecifier(TypeSpecifierPtr specifier);

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;
        TypeSpecifier GetType(Context &context) const;
        [[nodiscard]] const std::vector<TypeSpecifier> &GetSpecifiers() const;

    private:
        std::vector<TypeSpecifier> specifiers_;
    };
    using SpecifierQualifierListPtr = std::unique_ptr<const SpecifierQualifierList>;

}
