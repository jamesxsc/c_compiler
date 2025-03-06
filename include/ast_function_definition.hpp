#pragma once

#include "ast_node.hpp"
#include "ast_type_specifier.hpp"
#include "ast_function_declarator.hpp"
#include "ast_compound_statement.hpp"

namespace ast {

    class FunctionDefinition : public Node {
    private:
        DeclarationSpecifiersPtr declaration_specifiers_;
        FunctionDeclaratorPtr declarator_;
        NodePtr declaration_list_;
        CompoundStatementPtr compound_statement_;

    public:
        FunctionDefinition(DeclarationSpecifiersPtr declaration_specifiers, FunctionDeclaratorPtr declarator,
                           CompoundStatementPtr compound_statement)
                : declaration_specifiers_(std::move(declaration_specifiers)), declarator_(std::move(declarator)),
                  compound_statement_(std::move(compound_statement)) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        TypeSpecifier GetType(Context &context) const;
    };

} // namespace ast
