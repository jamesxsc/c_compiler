#pragma once

#include "ast_node.hpp"

namespace ast {

    class DirectDeclarator : public Node {
    private:
        std::string identifier_;
        // Note this is a basic implementation only supporting an identifier

    public:
        explicit DirectDeclarator(std::string identifier) : identifier_(std::move(identifier)) {};

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] const std::string& GetIdentifier() const;
    };

    using DirectDeclaratorPtr = std::unique_ptr<const DirectDeclarator>;

}
