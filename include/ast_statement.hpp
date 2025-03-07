#pragma once

#include <sstream>

#include "ast_node.hpp"
#include "ast_constant_expression.hpp"

namespace ast {

    // Serves as base abstract class and type for top-level 'statement'
    class Statement : public Node {
    public:
        virtual ~Statement() = default;

        virtual void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override = 0;

        virtual void Print(std::ostream &stream) const override = 0;

        [[nodiscard]] virtual bool IsCase() const;

        void SetInSwitchScope();

        [[nodiscard]] const std::vector<std::pair<std::string, std::reference_wrapper<const ConstantExpression>>> & GetSwitchLabelCasePairs() const;

    private:
        bool inSwitchScope_ = false;
        // Pair of label and ConstantExpressionPtr ref
        std::vector<std::pair<std::string, std::reference_wrapper<const ConstantExpression>>> switchLabelCasePairs_;
    };

    // Not const - has to be mutable
    using StatementPtr = std::unique_ptr<Statement>;

} // namespace ast
