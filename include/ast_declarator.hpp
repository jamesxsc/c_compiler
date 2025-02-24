#pragma once

#include "ast_node.hpp"

namespace ast {

    class Declarator : public Node {
    private:
        std::string identifier_;
        bool isDirect_;

    public:
        explicit Declarator(std::string identifier, bool isDirect) : identifier_(std::move(identifier)), isDirect_(isDirect) {};

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] const std::string& GetIdentifier() const;

        [[nodiscard]] bool IsDirect() const;

        void Indirect();
        void Direct();
    };

    using DeclaratorPtr = std::unique_ptr<const Declarator>;

}
