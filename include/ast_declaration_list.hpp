#pragma once

#include "ast_node.hpp"
#include "ast_declaration.hpp"

namespace ast {

    class DeclarationList : public Node {
    public:
        explicit DeclarationList(DeclarationPtr first) : declarations_() {
            if (first)
                declarations_.push_back(std::move(first));
        }

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        void PushBack(DeclarationPtr item);

        [[nodiscard]] size_t Size() const;

        [[nodiscard]] std::vector<DeclarationPtr>::const_iterator begin() const;

        [[nodiscard]] std::vector<DeclarationPtr>::const_iterator end() const;

    private:
        std::vector<DeclarationPtr> declarations_;
    };

    using DeclarationListPtr = std::unique_ptr<const DeclarationList>;

}
