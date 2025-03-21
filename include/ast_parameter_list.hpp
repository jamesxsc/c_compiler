#pragma once

#include <vector>
#include "ast_node.hpp"
#include "ast_parameter_declaration.hpp"

namespace ast {

    class ParameterList : public Node {
    public:
        explicit ParameterList(ParameterDeclarationPtr first);

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void EmitLabelRISC(std::ostream &stream) const;

        void Print(std::ostream &stream) const override;

        void PushBack(ParameterDeclarationPtr item);

        [[nodiscard]] size_t Size() const;

        [[nodiscard]] std::vector<ParameterDeclarationPtr>::const_iterator begin() const;

        [[nodiscard]] std::vector<ParameterDeclarationPtr>::const_iterator end() const;

        void SetHiddenPointerReturn() const;

    private:
        std::vector<ParameterDeclarationPtr> parameters_;
        mutable bool hiddenPointerReturn_{false};

        void EmitStructParameter(const TypeSpecifier &type, std::ostream &stream, Context &context, bool used,
                                 bool noIntRegs, bool noFloatRegs, int baseOffset, int &iidx, int &fidx,
                                 int &stackOffset, bool useStack) const;
    };

    using ParameterListPtr = std::unique_ptr<const ParameterList>;

}
