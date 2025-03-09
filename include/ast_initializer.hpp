#pragma once

#include "ast_node.hpp"
#include "ast_assignment_expression.hpp"
#include "ast_expression.hpp"

namespace ast {
    class Initializer : public Node {
        // TODO implement initializer list - possibly best to wait until structs
    public:
        explicit Initializer(AssignmentExpressionPtr expression): expression_(std::move(expression)) {}

        void EmitRISC(std::ostream& stream, Context& context, Register destReg) const override;
        void Print(std::ostream& stream) const override;

        [[nodiscard]] std::string GetGlobalIdentifier() const;
        [[nodiscard]] int GetGlobalValue() const;

    private:
        AssignmentExpressionPtr expression_;
    };

    using InitializerPtr = std::unique_ptr<const Initializer>;
}
