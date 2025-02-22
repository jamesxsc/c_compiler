#pragma once

#include "ast_node.hpp"
#include "ast_type_specifier.hpp"
#include "ast_direct_declarator.hpp"

namespace ast {

    class ParameterDeclaration : public Node
    {
    public:
        // TODO identifier type and possibilities
        ParameterDeclaration(TypeSpecifier type, DirectDeclaratorPtr identifier) : type_(type), identifier_(std::move(identifier)) {
            std::cout << "Declaration constructor called" << std::endl;
        }
        void EmitRISC(std::ostream &stream, Context &context, int destReg) const override;
        void Print(std::ostream &stream) const override;
        [[nodiscard]] const std::string &GetIdentifier() const;
    private:
        TypeSpecifier type_;
        DirectDeclaratorPtr identifier_;
    };

    using ParameterDeclarationPtr = std::unique_ptr<const ParameterDeclaration>;

} // namespace ast
