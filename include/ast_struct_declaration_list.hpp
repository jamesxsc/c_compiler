#pragma once

#include "ast_node.hpp"
#include "ast_struct_declaration.hpp"

namespace ast {

    class StructDeclarationList : public Node {
    public:
        explicit StructDeclarationList(StructDeclarationPtr first) {
            declarations_.push_back(std::move(first));
        }

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        void PushBack(StructDeclarationPtr declaration);

        [[nodiscard]] const std::vector<StructDeclarationPtr> &GetDeclarations() const;

        [[nodiscard]] std::vector<std::pair<std::string, TypeSpecifier>> GetMembers(Context *context = nullptr) const;

    private:
        std::vector<StructDeclarationPtr> declarations_;
    };

    using StructDeclarationListPtr = std::unique_ptr<StructDeclarationList>;

}
