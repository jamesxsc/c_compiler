#pragma once

#include "ast_node.hpp"
#include "ast_parameter_list.hpp"
#include "ast_declarator.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    class FunctionDeclarator final : public Declarator {
    private:
        // Identifier is stored in Declarator base class
        ParameterListPtr parameterList_;
        bool pointerReturn_{false};
        int indirectionLevel_{0};

    public:
        explicit FunctionDeclarator(DeclaratorPtr identifier) : Declarator(
                identifier->GetIdentifier(), true) {
            if (!identifier->IsDirect()) {
                throw std::runtime_error("Function identifier declarator must be direct");
            }
        };

        FunctionDeclarator(DeclaratorPtr identifier, ParameterListPtr parameters)
                : Declarator(identifier->GetIdentifier(), true), parameterList_(std::move(parameters)) {
            if (!identifier->IsDirect()) {
                throw std::runtime_error("Function identifier declarator must be direct");
            }
        };

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] bool IsFunction() const override;

        [[nodiscard]] Function BuildFunction(TypeSpecifier returnType, Context &context) const override;

        void EmitLabelRISC(std::ostream &stream) const;

        void SetPointerReturn(int indirectionLevel) override;

        bool GetPointerReturn() const;
    };

    using FunctionDeclaratorPtr = std::unique_ptr<const FunctionDeclarator>;

} // namespace ast
