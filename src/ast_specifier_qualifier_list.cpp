#include "ast_specifier_qualifier_list.hpp"

namespace ast {

    void SpecifierQualifierList::AddTypeSpecifier(TypeSpecifierPtr specifier) {
        specifiers_.push_back(*std::move(specifier));
    }

    void SpecifierQualifierList::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Probably nothing to do here
    }

    void SpecifierQualifierList::Print(std::ostream &stream) const {
        for (const auto &specifier : specifiers_) {
            stream << specifier << " ";
        }
    }

    TypeSpecifier SpecifierQualifierList::GetType(Context &context) const {

        // TODO gettype logic - support multiple specifiers, can this be shared with declaration_specifiers somehow? structs and enums may change this so wait till then
        return specifiers_.front();
    }

}
