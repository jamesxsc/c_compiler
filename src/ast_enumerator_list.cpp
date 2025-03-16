#include "ast_enumerator_list.hpp"

namespace ast {

    void EnumeratorList::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Nothing to do
    }

    void EnumeratorList::Print(std::ostream &stream) const {
        for (const auto &enumerator : enumerators_) {
            enumerator->Print(stream);
        }
    }

    void EnumeratorList::PushBack(EnumeratorPtr enumerator) {
        enumerators_.push_back(std::move(enumerator));
    }

    std::map<std::string, int> EnumeratorList::GetEnumerators() const {
        std::map<std::string, int> values{};
        for (const auto &enumerator : enumerators_) {
            if (enumerator->HasValue()) {
                values.emplace(enumerator->GetIdentifier(), enumerator->GetValue());
            } else {
                int nextValue = values.empty() ? 0 : values.rbegin()->second + 1;
                values.emplace(enumerator->GetIdentifier(), nextValue);
            }
        }
        return values;
    }

}
