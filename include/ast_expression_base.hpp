#pragma once

#include "ast_node.hpp"
#include "ast_type_specifier.hpp"  // <--- so we can return “Type”

namespace ast {

    class ExpressionBase : public Node
    {
    public:
        virtual ~ExpressionBase() = default;

        virtual void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override = 0;
        virtual void Print(std::ostream &stream) const override = 0;
        [[nodiscard]] virtual bool ContainsFunctionCall() const = 0;
        virtual Type GetType(Context &context) const = 0;
    };

    using ExpressionBasePtr = std::unique_ptr<const ExpressionBase>;

} // namespace ast
