#pragma once

#include "ast_node.hpp"
#include "ast_parameter_list.hpp"
#include "ast_direct_declarator.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    class FunctionDeclarator : public DirectDeclarator {
    private:
        ParameterListPtr parameterList_;

    public:
        Type GetType(Context &context) const;
        explicit FunctionDeclarator(DirectDeclaratorPtr identifier) : DirectDeclarator(
                identifier->GetIdentifier()) {};

        FunctionDeclarator(DirectDeclaratorPtr identifier, ParameterListPtr parameters)
                : DirectDeclarator(identifier->GetIdentifier()), parameterList_(std::move(parameters)) {};

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;
    };

    using FunctionDeclaratorPtr = std::unique_ptr<const FunctionDeclarator>;

} // namespace ast
