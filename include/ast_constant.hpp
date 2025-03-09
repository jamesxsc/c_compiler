#pragma once

#include "ast_node.hpp"
#include "ast_primary_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    class IntConstant : public PrimaryExpression {
    private:
        int value_;

    public:
        explicit IntConstant(int value) : value_(value) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        [[nodiscard]] int GetGlobalValue() const override;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;

        TypeSpecifier GetType(Context &context) const override;
    };

    class FloatConstant : public PrimaryExpression {
    private:
        double value_;

    public:
        explicit FloatConstant(double value) : value_(value) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        [[nodiscard]] int GetGlobalValue() const override;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;

        TypeSpecifier GetType(Context &context) const override;
    };


} // namespace ast
