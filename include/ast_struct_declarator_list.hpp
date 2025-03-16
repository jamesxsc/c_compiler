#pragma once

#include "ast_node.hpp"
#include "ast_struct_declarator.hpp"

namespace ast {

    class StructDeclaratorList : public Node {
    public:
        explicit StructDeclaratorList(StructDeclaratorPtr first) {
            declarators_.push_back(std::move(first));
        }

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        void PushBack(StructDeclaratorPtr declarator);

        [[nodiscard]] const std::vector<StructDeclaratorPtr> &GetDeclarators() const;

    private:
        std::vector<StructDeclaratorPtr> declarators_;
    };

    using StructDeclaratorListPtr = std::unique_ptr<StructDeclaratorList>;

}
