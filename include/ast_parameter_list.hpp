#pragma once

#include <vector>
#include "ast_node.hpp"
#include "ast_parameter_declaration.hpp"

namespace ast {

    class ParameterList : public Node {
    public:
        ParameterList(ParameterDeclarationPtr first);

        // Used for storing parameters on stack
        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        // Used for label args e.g. f(int, int, int):
        void EmitLabelRISC(std::ostream &stream) const;

        void Print(std::ostream &stream) const override;

        void PushBack(ParameterDeclarationPtr item);

        [[nodiscard]] size_t Size() const;

        [[nodiscard]] std::vector<ParameterDeclarationPtr>::const_iterator begin() const;

        [[nodiscard]] std::vector<ParameterDeclarationPtr>::const_iterator end() const;

    private:
        std::vector<ParameterDeclarationPtr> parameters_;
    };

    using ParameterListPtr = std::unique_ptr<const ParameterList>;

}
