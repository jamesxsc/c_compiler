#include "ast_compound_statement.hpp"

namespace ast {

    void CompoundStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (!isFunction_) { // Function scope is handled by declarator
            context.PushScope();
        }

        if (declarations_)
            declarations_->EmitRISC(stream, context, destReg);

        if (statements_) {
            statements_->SetInSwitchScope();

            statements_->EmitRISC(stream, context, destReg);

            // todo propogate to control blocks? + the setinscope
            LabelCasePairVector childPairs = statements_->GetSwitchLabelCasePairs();
            switchLabelCasePairs_.insert(std::end(switchLabelCasePairs_), std::begin(childPairs), std::end(childPairs));
        }

        if (!isFunction_) {
            context.PopScope(stream);
        }
    }

    void CompoundStatement::Print(std::ostream &stream) const {
        stream << "{\n";
        if (declarations_)
            declarations_->Print(stream);
        if (statements_)
            statements_->Print(stream);
        stream << "}\n";
    }

    bool CompoundStatement::IsFunction() const {
        return isFunction_;
    }

    void CompoundStatement::SetFunction() {
        isFunction_ = true;
    }

}
