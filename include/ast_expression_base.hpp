#pragma once

#include "ast_node.hpp"

// TODO is there actually any common behaviour between expression types
namespace ast {

     class ExpressionBase : public Node
    {
    public:
        virtual ~ExpressionBase() = default;
        virtual void EmitRISC(std::ostream &stream, Context &context, int destReg) const override = 0;
        virtual void Print(std::ostream &stream) const override = 0;
    };

    using ExpressionBasePtr = std::unique_ptr<const ExpressionBase>;

} // namespace ast
