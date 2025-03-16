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
        virtual bool ContainsFunctionCall() const = 0;
        virtual TypeSpecifier GetType(Context &context) const = 0;
        virtual int Evaluate() const = 0;
        virtual bool IsPointerDereference() const { return false; }
        virtual std::string GetGlobalIdentifier() const = 0;
        virtual int GetGlobalValue() const = 0;
    };

    using ExpressionBasePtr = std::unique_ptr<const ExpressionBase>;

} 
