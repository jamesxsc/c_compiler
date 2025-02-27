#pragma once

#include "ast_node.hpp"
#include "ast_type_specifier.hpp"
#include "ast_declarator.hpp"

namespace ast {

    class ParameterDeclaration : public Node
    {
    public:
        // TODO identifier type and possibilities
        ParameterDeclaration(TypeSpecifier type, DeclaratorPtr identifier) : type_(type), identifier_(std::move(identifier)) {};

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;
        [[nodiscard]] const std::string &GetIdentifier() const;
        Type GetType(Context &context) const;
    private:
        TypeSpecifier type_;
        DeclaratorPtr identifier_;
    };

    using ParameterDeclarationPtr = std::unique_ptr<const ParameterDeclaration>;

} // namespace ast
