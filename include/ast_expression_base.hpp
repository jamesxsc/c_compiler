#pragma once

#include "ast_node.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    class ExpressionBase : public Node
    {
    public:
        virtual ~ExpressionBase() = default;

        virtual void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override = 0;
        virtual void Print(std::ostream &stream) const override = 0;
        [[nodiscard]] virtual bool ContainsFunctionCall() const = 0;
        virtual TypeSpecifier GetType(Context &context) const = 0;
        // These are for global variables where we need a shortcut to initialize them
        // Calling these methods assume that the programmer is good
        virtual std::string GetGlobalIdentifier() const = 0;
        virtual int GetGlobalValue() const = 0;
    };

    using ExpressionBasePtr = std::unique_ptr<const ExpressionBase>;

} // namespace ast
