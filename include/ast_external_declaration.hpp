#pragma once

#include "ast_declaration.hpp"

namespace ast {

    // This class handles global/external function/variable declarations
    // NOT function definitions
    class ExternalDeclaration : public Declaration {
    public:
        explicit ExternalDeclaration(DeclarationPtr declaration) : Declaration(std::move(declaration)) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

    private:
        static void EmitStructInitializer(std::ostream &stream, Context &context, const InitializerList &initializerList,
                                   const TypeSpecifier &type);

        static void EmitArrayInitializer(std::ostream &stream, Context &context, const InitializerList &initializerList,
                                  const TypeSpecifier &type);
    };

}
