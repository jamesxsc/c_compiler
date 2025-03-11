#pragma once

#include "ast_declarator.hpp"
#include "ast_constant_expression.hpp"

namespace ast {

    class ArrayDeclarator : public Declarator {
    public:
        ArrayDeclarator(DeclaratorPtr identifier, ConstantExpressionPtr size) :
                Declarator(identifier->GetIdentifier(), true), size_(std::move(size)) {
            if (!identifier->IsDirect()) {
                throw std::runtime_error("Array identifier declarator must be direct");
            }
        }

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] bool IsArray() const override;

    private:
        ConstantExpressionPtr size_;
    };

}
