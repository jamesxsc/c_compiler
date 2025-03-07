#include "ast_labeled_statement.hpp"

namespace ast {

    void LabeledStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Labelling is handled in SwitchStatement
        statement_->EmitRISC(stream, context, destReg);
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
