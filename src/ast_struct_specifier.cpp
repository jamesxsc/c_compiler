#include "ast_struct_specifier.hpp"

namespace ast {

    // Only called for a definition
    void StructSpecifier::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        for (const auto &declaration : declarations_->GetMembers(context)) {
            members_.emplace_back(declaration.first, declaration.second);
        }

        if (isGlobal_) {
            context.InsertGlobalStruct(GetIdentifier(), members_);
        } else {
            context.CurrentFrame().structs.Insert(GetIdentifier(), members_);
        }


        // todo impl struct specifier
        // through context or otherwise this needs to be stored in typespecifier for getting size
        // or maybe we just somehow intercept calls to GetContext that return that... how much stuff holds a typespecifier?
        //  declaration specifiers and specifier qualifier list... hmmm
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

}
