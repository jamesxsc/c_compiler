#pragma once

#include "ast_node.hpp"
#include "ast_parameter_list.hpp"

namespace ast {

class FunctionDeclarator : public Node
{
private:
    NodePtr identifier_;
    ParameterListPtr parameters_;

public:
    FunctionDeclarator(NodePtr identifier) : identifier_(std::move(identifier)){};
    FunctionDeclarator(NodePtr identifier, ParameterListPtr parameters) : identifier_(std::move(identifier)), parameters_(std::move(parameters)){
        std::cout << "FunctionDeclarator constructor called" << std::endl;
    };

    void EmitRISC(std::ostream& stream, Context& context, int destReg) const override;
    void Print(std::ostream& stream) const override;
};

} // namespace ast
