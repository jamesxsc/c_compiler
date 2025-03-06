#pragma once

#include "ast_node.hpp"

namespace ast {

    enum class StorageClassSpecifier {
        None,
        Typedef,
        // Unsupported
//        Extern,
//        Static,
        Auto, // Do not confuse with modern C++ auto
        Register
    };

    std::ostream &operator<<(std::ostream &stream, StorageClassSpecifier storageClassSpecifier);

    class DeclarationSpecifiers : public Node {
    public:
        explicit DeclarationSpecifiers(StorageClassSpecifier storageClassSpecifier) :
            typeSpecifiers_() , storageClassSpecifier_(storageClassSpecifier) {}

        explicit DeclarationSpecifiers(TypeSpecifier first) : typeSpecifiers_({first}), storageClassSpecifier_(StorageClassSpecifier::None) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] const std::vector<TypeSpecifier> &GetTypeSpecifiers() const;

        [[nodiscard]] StorageClassSpecifier GetStorageClassSpecifier() const;

        void AddTypeSpecifier(TypeSpecifier typeSpecifier);

        void SetStorageClassSpecifier(StorageClassSpecifier storageClassSpecifier);
    private:
        std::vector<TypeSpecifier> typeSpecifiers_;
        // Supporting multiple is overkill for this project
        StorageClassSpecifier storageClassSpecifier_;
    };

    using DeclarationSpecifiersPtr = std::unique_ptr<const DeclarationSpecifiers>;

}
