#include "ast_statement.hpp"

namespace ast {

    bool Statement::IsCase() const {
        return false;
    }

    void Statement::SetInSwitchScope() {
        inSwitchScope_ = true;
    }

    const std::vector<std::pair<std::string, std::reference_wrapper<const ConstantExpression>>> &
    Statement::GetSwitchLabelCasePairs() const {
        return switchLabelCasePairs_;
    }

}
