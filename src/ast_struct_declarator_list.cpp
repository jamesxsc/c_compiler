#include "ast_struct_declarator_list.hpp"

namespace ast {

    void StructDeclaratorList::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Handled above this layer
    }

    void StructDeclaratorList::Print(std::ostream &stream) const {
        for (auto it = declarators_.begin(); it != declarators_.end(); ++it) {
            (*it)->Print(stream);
            if (it + 1 != declarators_.end()) {
                stream << ", ";
            }
        }
    }

    void StructDeclaratorList::PushBack(StructDeclaratorPtr declarator) {
        declarators_.push_back(std::move(declarator));
    }

    const std::vector<StructDeclaratorPtr> &StructDeclaratorList::GetDeclarators() const {
        return declarators_;
    }
}
