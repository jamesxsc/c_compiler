#pragma once

#include "ast_node.hpp"

// TODO is there actually any common behaviour between expression types
namespace ast {

    class Expression : public Node
    {
    public:
        virtual ~Expression() = default;
        virtual void EmitRISC(std::ostream &stream, Context &context, int destReg) const override = 0;
        virtual void Print(std::ostream &stream) const override = 0;
    };

    using ExpressionPtr = std::unique_ptr<const Expression>;

} // namespace ast
