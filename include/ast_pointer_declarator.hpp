#pragma once

#include "ast_declarator.hpp"

namespace ast {

    class PointerDeclarator : public Declarator {
    public:
        explicit PointerDeclarator(DeclaratorPtr declarator, int indirectionLevel) : Declarator(declarator->GetIdentifier(), false), indirectionLevel_(indirectionLevel) {};

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;

        [[nodiscard]] bool IsPointer() const override;

    private:
        int indirectionLevel_;
    };

}
