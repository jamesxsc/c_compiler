#pragma once

#include "ast_statement.hpp"
#include "ast_expression.hpp"

namespace ast {

    class SwitchStatement : public Statement {
    public:
        SwitchStatement(ExpressionPtr condition, StatementPtr body): condition_(std::move(condition)), body_(std::move(body)) {};

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] int RequiredStackSpace(Context &context) const override;

    private:
        ExpressionPtr condition_;
        StatementPtr body_;
    };

} // namespace ast
