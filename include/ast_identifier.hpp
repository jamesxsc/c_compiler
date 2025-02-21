#pragma once

#include <utility>

#include "ast_node.hpp"
#include "ast_primary_expression.hpp"

namespace ast {

    class Identifier : public PrimaryExpression {
    private:
        std::string identifier_;

    public:
        Identifier(std::string identifier) : identifier_(std::move(identifier)) {
            std::cout << "Identifier constructor called" << std::endl;
        };

        void EmitRISC(std::ostream &stream, Context &context, int destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] const std::string &GetIdentifier() const;
    };

} // namespace ast
