#pragma once

#include <sstream>
#include <vector>

#include "ast_node.hpp"
#include "ast_constant_expression.hpp"

namespace ast {

    // Serves as base abstract class and type for top-level 'statement'
    using LabelCasePairVector = std::vector<std::pair<std::string, std::optional<int>>>;

    class Statement : public Node {
    public:
        virtual ~Statement() = default;

        virtual void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override = 0;

        virtual void Print(std::ostream &stream) const override = 0;

        [[nodiscard]] virtual bool IsCase() const;

        void SetInSwitchScope() const;

        [[nodiscard]] const LabelCasePairVector &GetSwitchLabelCasePairs() const;

        [[nodiscard]] virtual int RequiredStackSpace(Context &context) const;

    protected:
        mutable bool inSwitchScope_ = false;
        // Note that this is populated by EmitRISC
        mutable LabelCasePairVector switchLabelCasePairs_;
    };

    using StatementPtr = std::unique_ptr<const Statement>;

} // namespace ast
