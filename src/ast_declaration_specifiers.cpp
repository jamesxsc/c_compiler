#include "ast_declaration_specifiers.hpp"
#include "risc_utils.hpp"

namespace ast {

    void DeclarationSpecifiers::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Do nothing
    }

    void DeclarationSpecifiers::Print(std::ostream &stream) const {
        if (storageClassSpecifier_ != StorageClassSpecifier::None) {
            stream << storageClassSpecifier_ << " ";
        }
        for (auto it = typeSpecifiers_.begin(); it != typeSpecifiers_.end(); ++it) {
            stream << *it;
            if (it != --typeSpecifiers_.end())
                stream << " ";
        }
    }

    StorageClassSpecifier DeclarationSpecifiers::GetStorageClassSpecifier() const {
        return storageClassSpecifier_;
    }

    void DeclarationSpecifiers::AddTypeSpecifier(TypeSpecifierPtr typeSpecifier) {
        typeSpecifiers_.push_back(*std::move(typeSpecifier));
    }

    void DeclarationSpecifiers::SetStorageClassSpecifier(StorageClassSpecifier storageClassSpecifier) {
        storageClassSpecifier_ = storageClassSpecifier;
    }

    TypeSpecifier DeclarationSpecifiers::GetType(Context &context) const {
        return Utils::ResolveTypeAlias(typeSpecifiers_);
    }


    std::ostream &operator<<(std::ostream &stream, StorageClassSpecifier storageClassSpecifier) {
        switch (storageClassSpecifier) {
            case StorageClassSpecifier::None:
                break;
            case StorageClassSpecifier::Typedef:
                stream << "typedef";
                break;
            case StorageClassSpecifier::Auto:
                stream << "auto";
                break;
            case StorageClassSpecifier::Register:
                stream << "register";
                break;
        }
        return stream;
    }
}
