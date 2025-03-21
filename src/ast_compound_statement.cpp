#include "ast_compound_statement.hpp"

namespace ast {

    void CompoundStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (!isFunction_) { // Function scope is handled by declarator
            context.PushScope();
        }

        if (declarations_)
            declarations_->EmitRISC(stream, context, destReg);

        if (statements_) {
            if (inSwitchScope_)
                statements_->SetInSwitchScope();

            statements_->EmitRISC(stream, context, destReg);

            if (inSwitchScope_) {
                LabelCasePairVector childPairs = statements_->GetSwitchLabelCasePairs();
                switchLabelCasePairs_.insert(std::end(switchLabelCasePairs_), std::begin(childPairs),
                                             std::end(childPairs));
            }
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

    int CompoundStatement::RequiredStackSpace(Context &context) const {
        int space = 0;
        if (declarations_)
            for (const auto &declaration : *declarations_)
                space += declaration->RequiredStackSpace(context);
        if (statements_)
            for (const auto &statement : *statements_)
                space += statement->RequiredStackSpace(context);
        return space;
    }

}
