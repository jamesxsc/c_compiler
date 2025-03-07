#pragma once

#include <optional>

#include "ast_statement.hpp"
#include "ast_constant_expression.hpp"

namespace ast {

    class LabeledStatement : public Statement {
    public:
        LabeledStatement(StatementPtr statement, std::string identifier)
                : statement_(std::move(statement)), identifier_(std::move(identifier)), case_(), defaultCase_(false) {};

        LabeledStatement(StatementPtr statement, ConstantExpressionPtr caseExpr)
                : statement_(std::move(statement)), identifier_(nullptr), case_(std::move(caseExpr)),
                  defaultCase_(false) {};

        explicit LabeledStatement(StatementPtr statement)
                : statement_(std::move(statement)), identifier_(nullptr), case_(), defaultCase_(true) {};

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] bool IsCase() const override;

    private:
        StatementPtr statement_;
        std::optional<std::string> identifier_;
        ConstantExpressionPtr case_;
        bool defaultCase_;
    };

}
