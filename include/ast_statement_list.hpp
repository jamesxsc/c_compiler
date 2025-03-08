#pragma once

#include "ast_node.hpp"
#include "ast_statement.hpp"

namespace ast {

    class StatementList : public Node {
    public:
        explicit StatementList(StatementPtr first) : statements_() {
            statements_.push_back(std::move(first));
        }

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        void PushBack(StatementPtr item);

        [[nodiscard]] size_t Size() const;

        [[nodiscard]] std::vector<StatementPtr>::const_iterator begin() const;

        [[nodiscard]] std::vector<StatementPtr>::const_iterator end() const;

        [[nodiscard]] LabelCasePairVector GetSwitchLabelCasePairs() const;

        void SetInSwitchScope() const;

    private:
        std::vector<StatementPtr> statements_;
    };

    using StatementListPtr = std::unique_ptr<const StatementList>;

} // namespace ast
