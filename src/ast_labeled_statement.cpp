#include "ast_labeled_statement.hpp"

namespace ast {

    void LabeledStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (inSwitchScope_) {
            std::string label = context.MakeLabel(".CASE");
            std::pair<std::string, ConstantExpressionPtr> pair = std::pair(label, case_);
            switchLabelCasePairs_.push_back(pair);

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

        // todo if we need a null statement type or if nullptr is ok
        if (statement_) {
            statement_->Print(stream);
        }
    }

    bool LabeledStatement::IsCase() const {
        return case_ != nullptr || defaultCase_;
    }

}
