#pragma once

#include "ast_statement.hpp"
#include "ast_statement_list.hpp"
#include "ast_declaration_list.hpp"

namespace ast {

    class CompoundStatement : public Statement {
    public:
        CompoundStatement(DeclarationListPtr declarations, StatementListPtr statements): declarations_(std::move(declarations)), statements_(std::move(statements)) {};

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

    private:
        // Both are optional
        DeclarationListPtr declarations_;
        StatementListPtr statements_;
    };

    using CompoundStatementPtr = std::unique_ptr<const CompoundStatement>;

} // namespace ast
