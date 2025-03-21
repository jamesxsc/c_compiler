#pragma once

#include "ast_declaration.hpp"
#include "ast_enum_specifier.hpp"
#include "ast_struct_specifier.hpp"

namespace ast {

    class AggregateTypeDefinition : public Declaration {
    public:
        explicit AggregateTypeDefinition(EnumSpecifierPtr enumSpecifier) : Declaration(nullptr, nullptr),
                                                                           enumSpecifier_(std::move(enumSpecifier)) {}

        explicit AggregateTypeDefinition(StructSpecifierPtr structSpecifier) : Declaration(nullptr, nullptr),
                                                                               structSpecifier_(
                                                                                       std::move(structSpecifier)) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] bool IsAggregateTypeDefinition() const override;

        void SetGlobal();

    private:
        EnumSpecifierPtr enumSpecifier_;
        StructSpecifierPtr structSpecifier_;
        bool isGlobal_{false};
    };

}
