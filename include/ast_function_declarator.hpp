#pragma once

#include "ast_node.hpp"

namespace ast {

class FunctionDeclarator : public Node
{
private:
    NodePtr identifier_;
    NodeListPtr parameters_;

public:
    FunctionDeclarator(NodePtr identifier) : identifier_(std::move(identifier)){};
    FunctionDeclarator(NodePtr identifier, NodeListPtr parameters) : identifier_(std::move(identifier)), parameters_(std::move(parameters)){
        std::cout << "FunctionDeclarator constructor called" << std::endl;
    };

    void EmitRISC(std::ostream& stream, Context& context, int destReg) const override;
    void Print(std::ostream& stream) const override;
};

} // namespace ast
