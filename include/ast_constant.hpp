#pragma once

#include <cassert>
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

        [[nodiscard]] int Evaluate() const override;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        [[nodiscard]] int GetGlobalValue() const override;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;

        TypeSpecifier GetType(Context &context) const override;
    };

    class CharConstant : public PrimaryExpression {
    private:
        int value_;

    public:
        explicit CharConstant(int value) : value_(value) {
            assert((value & 0xffffff00) == 0);
        }

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] int Evaluate() const override;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        [[nodiscard]] int GetGlobalValue() const override;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;

        TypeSpecifier GetType(Context &context) const override;
    };

    class FloatConstant : public PrimaryExpression {
    private:
        double value_;
        bool doublePrecision_;

    public:
        FloatConstant(double value, bool doublePrecision) : value_(value), doublePrecision_(doublePrecision) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] int Evaluate() const override;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        [[nodiscard]] int GetGlobalValue() const override;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;

        TypeSpecifier GetType(Context &context) const override;
    };

    class StringConstant : public PrimaryExpression {
    private:
        std::string value_;

    public:
        explicit StringConstant(std::string value) : value_(std::move(value)) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] int Evaluate() const override;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        [[nodiscard]] int GetGlobalValue() const override;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;

        TypeSpecifier GetType(Context &context) const override;
    };

} // namespace ast
