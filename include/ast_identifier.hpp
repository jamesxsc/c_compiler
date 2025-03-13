#pragma once

#include <utility>

#include "ast_node.hpp"
#include "ast_primary_expression.hpp"

namespace ast {

    class Identifier : public PrimaryExpression {
    private:
        std::string identifier_;

    public:
        TypeSpecifier GetType(Context &context) const override;
        explicit Identifier(std::string identifier) : identifier_(std::move(identifier)) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        [[nodiscard]] const std::string &GetIdentifier() const;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;

        [[nodiscard]] int GetGlobalValue() const override;
    };

} // namespace ast
