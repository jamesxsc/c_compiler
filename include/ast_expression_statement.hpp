#pragma once

#include "ast_statement.hpp"
#include "ast_expression.hpp"

namespace ast {

    class ExpressionStatement : public Statement {
    public:
        ExpressionStatement() : expression_(nullptr) {};

        explicit ExpressionStatement(ExpressionPtr expression) : expression_(std::move(expression)) {};

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

    private:
        ExpressionPtr expression_;
    };

} // namespace ast
