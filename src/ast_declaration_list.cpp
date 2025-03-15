#include "ast_declaration_list.hpp"

namespace ast {

    void DeclarationList::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        for (const auto &declaration: declarations_) {
            declaration->EmitRISC(stream, context, destReg);
        }
    }

    void DeclarationList::Print(std::ostream &stream) const {
        for (const auto &declaration: declarations_) {
            declaration->Print(stream);
        }
    }

    void DeclarationList::PushBack(DeclarationPtr item) {
        if (item)
            declarations_.push_back(std::move(item));
    }

    size_t DeclarationList::Size() const {
        return declarations_.size();
    }

    std::vector<DeclarationPtr>::const_iterator DeclarationList::begin() const {
        return declarations_.begin();
    }

    std::vector<DeclarationPtr>::const_iterator DeclarationList::end() const {
        return declarations_.end();
    }
}
