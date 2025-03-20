#pragma once

#include "ast_node.hpp"
#include "ast_expression.hpp"
#include "ast_assignment_expression.hpp"

namespace ast {

    class ArgumentExpressionList : public Node {
    public:
        explicit ArgumentExpressionList(AssignmentExpressionPtr first) {
            arguments_.push_back(std::move(first));
        }

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;

        void PushBack(AssignmentExpressionPtr item);

        [[nodiscard]] size_t Size() const;
        [[nodiscard]] std::vector<AssignmentExpressionPtr>::const_iterator begin() const;
        [[nodiscard]] std::vector<AssignmentExpressionPtr>::const_iterator end() const;
    private:
        std::vector<AssignmentExpressionPtr> arguments_;

        void
        EmitStructArgument(std::ostream &stream, Context &context, Register &firstIntegerReg, Register &firstFloatReg,
                           int &stackOffset, const TypeSpecifier &type, int &memberOffset,
                           const Register &baseAddressReg) const;
    };

    using ArgumentExpressionListPtr = std::unique_ptr<const ArgumentExpressionList>;

}
