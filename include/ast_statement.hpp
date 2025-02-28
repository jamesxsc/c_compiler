#pragma once

#include "ast_node.hpp"

namespace ast {

    // Serves as base abstract class and type for top-level 'statement'
    class Statement : public Node {
    public:
        virtual ~Statement() = default;


        virtual void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override = 0;

        virtual void Print(std::ostream &stream) const override = 0;
    };

    using StatementPtr = std::unique_ptr<const Statement>;

} // namespace ast
