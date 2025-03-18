#include "ast_init_declarator_list.hpp"

namespace ast {

    InitDeclaratorList::InitDeclaratorList(InitDeclaratorPtr first): initDeclarators_() {
        initDeclarators_.push_back(std::move(first));
    }

    void InitDeclaratorList::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // These are handled in declaration
    }

    void InitDeclaratorList::Print(std::ostream &stream) const {
        if (!initDeclarators_.empty()) {
            for (auto it = initDeclarators_.begin(); it != initDeclarators_.end(); ++it) {
                (*it)->Print(stream);
                if (it + 1 != initDeclarators_.end()) {
                    stream << ", ";
                }
            }
        }
    }

    void InitDeclaratorList::PushBack(InitDeclaratorPtr item) {
        initDeclarators_.push_back(std::move(item));
    }

    size_t InitDeclaratorList::Size() const {
        return initDeclarators_.size();
    }

    std::vector<InitDeclaratorPtr>::const_iterator InitDeclaratorList::begin() const {
        return initDeclarators_.begin();
    }

    std::vector<InitDeclaratorPtr>::const_iterator InitDeclaratorList::end() const {
        return initDeclarators_.end();
    }
}
