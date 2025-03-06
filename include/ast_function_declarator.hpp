#pragma once

#include "ast_node.hpp"
#include "ast_parameter_list.hpp"
#include "ast_declarator.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    class FunctionDeclarator : public Declarator {
    private:
        // Identifier is stored in Declarator base class
        ParameterListPtr parameterList_;

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

        [[nodiscard]] Function BuildFunction(TypeSpecifier returnType) const override;

        void EmitLabelRISC(std::ostream &stream) const;
    };

    using FunctionDeclaratorPtr = std::unique_ptr<const FunctionDeclarator>;

} // namespace ast
