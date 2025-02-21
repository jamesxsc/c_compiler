#include "ast_parameter_list.hpp"
#include "ast_context.hpp"

namespace ast {

    ParameterList::ParameterList(ParameterDeclarationPtr first) : parameters_() {
        parameters_.push_back(std::move(first));
    }

    // TODO populate stubs
    void ParameterList::EmitRISC(std::ostream &stream, Context &context, int destReg) const {

    }

    void ParameterList::Print(std::ostream &stream) const {

    }

    void ParameterList::PushBack(ParameterDeclarationPtr item) {
        parameters_.push_back(std::move(item));
    }

    size_t ParameterList::Size() const {
        return parameters_.size();
    }

    std::vector<ParameterDeclarationPtr>::const_iterator ParameterList::begin() const {
        return parameters_.begin();
    }

    std::vector<ParameterDeclarationPtr>::const_iterator ParameterList::end() const {
        return parameters_.end();
    }

}
