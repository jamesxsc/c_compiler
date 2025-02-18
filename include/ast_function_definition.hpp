#pragma once

#include "ast_node.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

class FunctionDefinition : public Node
{
private:
    const TypeSpecifier declaration_specifiers_;
    NodePtr declarator_;
    NodePtr declaration_list_;
    NodePtr compound_statement_;

public:
    FunctionDefinition(TypeSpecifier declaration_specifiers, NodePtr declarator, NodePtr compound_statement) : declaration_specifiers_(declaration_specifiers), declarator_(std::move(declarator)), compound_statement_(std::move(compound_statement)){};
    FunctionDefinition(TypeSpecifier declaration_specifiers, NodePtr declarator, NodePtr declaration_list, NodePtr compound_statement) : declaration_specifiers_(declaration_specifiers), declarator_(std::move(declarator)), declaration_list_(std::move(declaration_list)), compound_statement_(std::move(compound_statement)) {}

    void EmitRISC(std::ostream& stream, Context& context) const override;
    void Print(std::ostream& stream) const override;
};

} // namespace ast
