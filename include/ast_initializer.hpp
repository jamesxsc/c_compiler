#pragma once

#include "ast_node.hpp"
#include "ast_assignment_expression.hpp"
#include "ast_expression.hpp"

namespace ast {

    class Initializer;

    using InitializerPtr = std::unique_ptr<const Initializer>;

    class Initializer : public Node {
    public:
        explicit Initializer(AssignmentExpressionPtr expression) : expression_(std::move(expression)) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] std::string GetGlobalIdentifier() const;

        template<class T>
        [[nodiscard]] T Evaluate(Context &context) const;

        [[nodiscard]] virtual bool IsList() const;

        [[nodiscard]] virtual int ListSize() const;

        virtual void AddInitializer(InitializerPtr initializer);

    private:
        AssignmentExpressionPtr expression_; // Null for a list
    };

}
