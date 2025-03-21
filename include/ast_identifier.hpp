#pragma once

#include <utility>

#include "ast_node.hpp"
#include "ast_primary_expression.hpp"

namespace ast {

    class Identifier : public PrimaryExpression {
    private:
        std::string identifier_;

    public:
        TypeSpecifier GetType(Context &context) const override;
        explicit Identifier(std::string identifier) : identifier_(std::move(identifier)) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] double EvaluateFloat(Context &context) const override;
        [[nodiscard]] int Evaluate(Context &context) const override;

        [[nodiscard]] bool ContainsFunctionCall() const override;

        [[nodiscard]] const std::string &GetIdentifier() const;

        [[nodiscard]] std::string GetGlobalIdentifier() const override;

        static void
        EmitStructReturnInRegisters(Context &context, std::ostream &stream, int offset, Register &intReg,
                                    Register &floatReg,
                                    const std::vector<std::pair<std::string, TypeSpecifier>> &structMembers,
                                    const Register &loadAddressReg = Register::zero);
    };

} // namespace ast
