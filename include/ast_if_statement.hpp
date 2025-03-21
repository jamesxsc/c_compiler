#pragma once

#include "ast_statement.hpp"
#include "ast_expression.hpp"

namespace ast {

//--------------------------------------
// IfStatement
//--------------------------------------
    class IfStatement : public Statement {
    public:
        IfStatement(ExpressionPtr condition, StatementPtr thenStmt, StatementPtr elseStmt = nullptr) : condition_(
                std::move(condition)), thenStmt_(std::move(thenStmt)), elseStmt_(std::move(elseStmt)) {};

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

    private:
        ExpressionPtr condition_;
        StatementPtr thenStmt_;
        StatementPtr elseStmt_;
    };

} // namespace ast
