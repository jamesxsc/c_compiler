#pragma once

#include "ast_node.hpp"
#include "ast_enumerator.hpp"

namespace ast {

    class EnumeratorList : public Node {
    public:
        explicit EnumeratorList(EnumeratorPtr first) : enumerators_() {
            enumerators_.push_back(std::move(first));
        }

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        void PushBack(EnumeratorPtr enumerator);

        [[nodiscard]] std::map<std::string, int> GetEnumerators(Context &context) const;

    private:
        std::vector<EnumeratorPtr> enumerators_;
    };
    using EnumeratorListPtr = std::unique_ptr<const EnumeratorList>;

}
