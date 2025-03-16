#pragma once

#include "ast_declarator.hpp"

namespace ast {

    class StructDeclarator : public Declarator {
    public:
        explicit StructDeclarator(DeclaratorPtr declarator) : Declarator(declarator->GetIdentifier(), false) {};

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;

    };

    using StructDeclaratorPtr = std::unique_ptr<const StructDeclarator>;

}
