#include "ast_statement_list.hpp"

namespace ast {

    void StatementList::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        for (const auto& statement : statements_) {
            statement->EmitRISC(stream, context, destReg);
        }
    }

    void StatementList::Print(std::ostream &stream) const {
        for (const auto& statement : statements_) {
            statement->Print(stream);
        }
    }

    void StatementList::PushBack(StatementPtr item) {
        statements_.push_back(std::move(item));
    }

    size_t StatementList::Size() const {
        return statements_.size();
    }

    std::vector<StatementPtr>::const_iterator StatementList::begin() const {
        return statements_.begin();
    }

    std::vector<StatementPtr>::const_iterator StatementList::end() const {
        return statements_.end();
    }

    LabelCasePairVector StatementList::GetSwitchLabelCasePairs() const {
        LabelCasePairVector pairs;
        for (const auto& statement : statements_) {
            auto childPairs = statement->GetSwitchLabelCasePairs();
            pairs.insert(std::end(pairs), std::begin(childPairs), std::end(childPairs));
        }
        return pairs;
    }

    void StatementList::SetInSwitchScope() const {
        for (const auto& statement : statements_) {
            statement->SetInSwitchScope();
        }
    }

} // namespace ast
