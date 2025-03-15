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

    std::unordered_map<std::string, int> EnumeratorList::GetEnumerators() const {
        std::unordered_map<std::string, int> values{};
        int nextValue = 0;
        for (const auto &enumerator : enumerators_) {
            if (enumerator->HasValue()) {
                values.emplace(enumerator->GetIdentifier(), enumerator->GetValue());
                nextValue = enumerator->GetValue() + 1;
            } else {
                values.emplace(enumerator->GetIdentifier(), nextValue++);
            }
        }
        return values;
    }

}
