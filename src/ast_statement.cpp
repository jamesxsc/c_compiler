#include "ast_statement.hpp"

namespace ast {

    bool Statement::IsCase() const {
        return false;
    }

    void Statement::SetInSwitchScope() const {
        inSwitchScope_ = true;
    }

    const LabelCasePairVector &Statement::GetSwitchLabelCasePairs() const {
        return switchLabelCasePairs_;
    }

    int Statement::RequiredStackSpace(Context &context) const {
        return 0; // Standard statement requires none
    }

}
