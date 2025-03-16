#include "ast_labeled_statement.hpp"

namespace ast {

    void LabeledStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (inSwitchScope_) {
            std::string label = context.MakeLabel(".CASE");
            std::optional<int> value = case_ ? std::optional<int>{case_->Evaluate(context)} : std::nullopt;
            std::pair<std::string, std::optional<int>> pair = std::pair(label, value);
            switchLabelCasePairs_.emplace_back(pair);
            stream << label << ":" << std::endl;
        } // Otherwise ignore a labelled statement
        statement_->EmitRISC(stream, context, destReg);

        LabelCasePairVector childPairs = statement_->GetSwitchLabelCasePairs();
        switchLabelCasePairs_.insert(std::end(switchLabelCasePairs_), std::begin(childPairs), std::end(childPairs));
    }

    void LabeledStatement::Print(std::ostream &stream) const {
        // This isn't strictly necessary since goto is unsupported
        if (identifier_) {
            stream << *identifier_ << ": ";
        } else if (case_) {
            stream << "case ";
            case_->Print(stream);
            stream << ": ";
        } else if (defaultCase_) {
            stream << "default: ";
        }

        statement_->Print(stream);
    }

    bool LabeledStatement::IsCase() const {
        return case_ != nullptr || defaultCase_;
    }

}
