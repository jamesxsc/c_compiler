#include "ast_struct_specifier.hpp"

namespace ast {

    // Only called for a definition by agg type def to insert into context
    void StructSpecifier::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        for (const auto &declaration: declarations_->GetMembers(&context)) {
            members_.emplace_back(declaration.first, declaration.second);
        }

        if (isGlobal_) {
            if (HasIdentifier()) // Do not store anonymous structs
                context.InsertGlobalStruct(GetIdentifier(), members_);
        } else {
            context.CurrentFrame().structs.Insert(GetIdentifier(), members_);
        }
    }

    void StructSpecifier::Print(std::ostream &stream) const {
        stream << "struct ";
        if (HasIdentifier()) {
            stream << GetIdentifier();
        }
        if (HasDeclarations()) {
            stream << " {" << std::endl;
            declarations_->Print(stream);
            stream << "}";
        }
    }

    bool StructSpecifier::HasIdentifier() const {
        return identifier_.has_value();
    }

    const std::string &StructSpecifier::GetIdentifier() const {
        return *identifier_;
    }

    bool StructSpecifier::HasDeclarations() const {
        return declarations_ != nullptr;
    }

    void StructSpecifier::SetGlobal() {
        isGlobal_ = true;
    }

    // Used for anonymous structs
    std::vector<std::pair<std::string, TypeSpecifier>> StructSpecifier::GetKnownMembers() const {
        if (!HasDeclarations()) {
            // Don't like this
            return {};
        }
        // There's no point caching etc. because each type specifier has it's own instance of this
        std::vector<std::pair<std::string, TypeSpecifier>> members;
        for (const auto &declaration: declarations_->GetMembers()) {
            members.emplace_back(declaration.first, declaration.second);
        }
        return members;
    }

}
