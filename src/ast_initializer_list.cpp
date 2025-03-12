#include "ast_initializer_list.hpp"

namespace ast {

    void InitializerList::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Never called on a list
    }

    void InitializerList::Print(std::ostream &stream) const {
        stream << "{";
        for (auto it = initializers_.begin(); it != initializers_.end(); ++it) {
            (*it)->Print(stream);
            if (it + 1 != initializers_.end()) {
                stream << ", ";
            }
        }
        stream << "}";
    }

    bool InitializerList::IsList() const {
        return true;
    }

    InitializerList::~InitializerList() = default;

    void InitializerList::AddInitializer(InitializerPtr initializer) {
        initializers_.push_back(std::move(initializer));
    }

    std::vector<InitializerPtr>::const_iterator InitializerList::begin() const {
        return initializers_.begin();
    }

    std::vector<InitializerPtr>::const_iterator InitializerList::end() const {
        return initializers_.end();
    }

}
