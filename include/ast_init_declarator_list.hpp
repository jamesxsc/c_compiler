#pragma once

#include "ast_node.hpp"
#include "ast_init_declarator.hpp"

namespace ast {

    class InitDeclaratorList : public Node {
    public:
        explicit InitDeclaratorList(InitDeclaratorPtr first);

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        void PushBack(InitDeclaratorPtr item);

        [[nodiscard]] size_t Size() const;

        [[nodiscard]] std::vector<InitDeclaratorPtr>::const_iterator begin() const;

        [[nodiscard]] std::vector<InitDeclaratorPtr>::const_iterator end() const;
    private:
        std::vector<InitDeclaratorPtr> initDeclarators_;
    };

    using InitDeclaratorListPtr = std::unique_ptr<const InitDeclaratorList>;

}
