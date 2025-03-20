#pragma once

#include "ast_node.hpp"
#include "ast_struct_declaration_list.hpp"

namespace ast {

    class StructSpecifier : public Node {
    public:
        explicit StructSpecifier(std::string identifier, StructDeclarationListPtr declarations)
                : identifier_(std::move(identifier)), declarations_(std::move(declarations)) {}

        explicit StructSpecifier(StructDeclarationListPtr declarations)
                : identifier_(std::nullopt), declarations_(std::move(declarations)) {}

        explicit StructSpecifier(std::string identifier)
                : identifier_(std::move(identifier)), declarations_(nullptr) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] bool HasIdentifier() const;

        [[nodiscard]] const std::string &GetIdentifier() const;

        [[nodiscard]] bool HasDeclarations() const;

        void SetGlobal();

        [[nodiscard]] std::vector<std::pair<std::string, TypeSpecifier>> GetKnownMembers() const;

    private:
        std::optional<std::string> identifier_;
        StructDeclarationListPtr declarations_;
        mutable std::vector<std::pair<std::string, TypeSpecifier>> members_;
        bool isGlobal_{false};
    };

    using StructSpecifierPtr = std::unique_ptr<StructSpecifier>;

}
