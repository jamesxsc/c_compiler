#include "ast_declaration_specifiers.hpp"

namespace ast {

    void DeclarationSpecifiers::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Do nothing
    }

    void DeclarationSpecifiers::Print(std::ostream &stream) const {
        if (storageClassSpecifier_ != StorageClassSpecifier::None) {
            stream << storageClassSpecifier_ << " ";
        }
        for (const auto &typeSpecifier : typeSpecifiers_) {
            stream << typeSpecifier << " ";
        }
    }

    const std::vector<TypeSpecifier> &DeclarationSpecifiers::GetTypeSpecifiers() const {
        return typeSpecifiers_;
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
        return typeSpecifiers_.front();
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
