#include "ast_enum_specifier.hpp"

#include <unordered_map>
#include <cassert>

namespace ast {

    void EnumSpecifier::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Nothing to do
        // Not even put in context since typespecifier contains the definition
    }

    void EnumSpecifier::Print(std::ostream &stream) const {
        stream << "enum ";
        if (HasIdentifier()) {
            stream << GetIdentifier();
        }
        if (HasEnumerators()) {
            stream << " { ";
            enumerators_->Print(stream);
            stream << "}";
        }
    }

    bool EnumSpecifier::HasIdentifier() const {
        return identifier_.has_value();
    }

    const std::string &EnumSpecifier::GetIdentifier() const {
        return *identifier_;
    }

    bool EnumSpecifier::HasEnumerators() const {
        return enumerators_ != nullptr;
    }

    static std::unordered_map<std::string, TypeSpecifier> definedEnums{};

    // Called by parser in type_name rule
    TypeSpecifier EnumSpecifier::GetTypeSpecifier() const {
        // Order matters, so don't populate in constructor
        // I'd rather put this here than clutter the parser
        // No definition
        if (!HasEnumerators()) {
            assert(HasIdentifier() &&
                   "EnumSpecifier::GetTypeSpecifier() called on an enum without enumerators or identifier");
            if (definedEnums.find(GetIdentifier()) != definedEnums.end())
                return definedEnums.at(*identifier_);
            else
                throw std::runtime_error("EnumSpecifier::GetTypeSpecifier() called on an undefined enum");
        }

        // Definition
        TypeSpecifier typeSpecifier{GetIdentifier(), enumerators_->GetEnumerators()};

        // Store for future usages
        if (HasIdentifier())
            definedEnums.emplace(GetIdentifier(), typeSpecifier);

        return typeSpecifier;
    }

}
