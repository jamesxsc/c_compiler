#pragma once

#include "ast_node.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    class Declaration : public Node
    {
    public:
        // TODO identifier type and possibilities
        Declaration(TypeSpecifier type, NodePtr identifier) : type_(type), identifier_(std::move(identifier)) {
            std::cout << "Declaration constructor called" << std::endl;
        }
        void EmitRISC(std::ostream &stream, Context &context, int destReg) const override;
        void Print(std::ostream &stream) const override;
        [[nodiscard]] const NodePtr &GetIdentifier() const;
    private:
        TypeSpecifier type_;
        NodePtr identifier_;
    };

} // namespace ast
