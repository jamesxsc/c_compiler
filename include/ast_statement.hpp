#pragma once

#include <sstream>
#include <vector>

#include "ast_node.hpp"
#include "ast_constant_expression.hpp"

namespace ast {

    // Serves as base abstract class and type for top-level 'statement'
    using LabelCasePairVector = std::vector<std::pair<std::string, ConstantExpressionPtr>>;
    class Statement : public Node {
    public:
        virtual ~Statement() = default;

        virtual void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override = 0;

        virtual void Print(std::ostream &stream) const override = 0;

        [[nodiscard]] virtual bool IsCase() const;

        void SetInSwitchScope() const;

        [[nodiscard]] const LabelCasePairVector & GetSwitchLabelCasePairs() const;

    protected:
        mutable bool inSwitchScope_ = false;
        // Pair of label and ConstantExpressionPtr (shared for this purpose)
        // Note that this is populated by EmitRISC
        mutable LabelCasePairVector switchLabelCasePairs_;
    };

    // Not const - has to be mutable todo check since we made the members mutable
    using StatementPtr = std::unique_ptr<Statement>;

} // namespace ast
