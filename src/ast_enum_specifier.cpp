#include "ast_enum_specifier.hpp"

#include <unordered_map>
#include <cassert>

namespace ast {

    // Only called for a definition
    void EnumSpecifier::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        for (const auto &enumerator : enumerators_->GetEnumerators(context)) {
            enumeratorsValuesMap_.emplace(enumerator.first, enumerator.second);
        }

        if (isGlobal_) {
            if (HasIdentifier())
                context.InsertGlobalEnum(GetIdentifier(), enumeratorsValuesMap_);
            else
                context.InsertGlobalEnum(enumeratorsValuesMap_);
        } else {
            context.CurrentFrame().enums.Insert(GetIdentifier(), enumeratorsValuesMap_);
        }
    }

    void EnumSpecifier::Print(std::ostream &stream) const {
        stream << "enum ";
        if (HasIdentifier()) {
            stream << GetIdentifier();
        }
        if (HasEnumerators()) {
            stream << " {" << std::endl;
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

    void EnumSpecifier::SetGlobal() {
        isGlobal_ = true;
    }

}
