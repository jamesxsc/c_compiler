#pragma once

#include "ast_node.hpp"
#include "ast_type_specifier.hpp"
#include "ast_declarator.hpp"
#include "ast_declaration_specifiers.hpp"

namespace ast {

    class ParameterDeclaration : public Node
    {
    public:
        ParameterDeclaration(DeclarationSpecifiersPtr declarationSpecifiers, DeclaratorPtr identifier) : declarationSpecifiers_(std::move(declarationSpecifiers)), identifier_(std::move(identifier)) {};
        explicit ParameterDeclaration(DeclarationSpecifiersPtr declarationSpecifiers) : declarationSpecifiers_(std::move(declarationSpecifiers)), identifier_(nullptr) {};

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;
        [[nodiscard]] const std::string &GetIdentifier() const;
        TypeSpecifier GetType(Context &context) const;
        bool HasIdentifier() const;
    private:
        DeclarationSpecifiersPtr declarationSpecifiers_;

        DeclaratorPtr identifier_;
    };

    using ParameterDeclarationPtr = std::unique_ptr<const ParameterDeclaration>;

} // namespace ast
