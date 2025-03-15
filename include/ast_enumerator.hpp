#pragma once

#include "ast_node.hpp"
#include "ast_constant_expression.hpp"

namespace ast {

    class Enumerator : public Node {
    public:
        explicit Enumerator(std::string identifier) : identifier_(std::move(identifier)), value_(nullptr) {};

        Enumerator(std::string identifier, ConstantExpressionPtr value)
                : identifier_(std::move(identifier)), value_(std::move(value)) {};

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;

        [[nodiscard]] std::string GetIdentifier() const;
        [[nodiscard]] bool HasValue() const;
        [[nodiscard]] int GetValue() const;

    private:
        std::string identifier_;
        ConstantExpressionPtr value_;
    };
    using EnumeratorPtr = std::unique_ptr<const Enumerator>;

}
