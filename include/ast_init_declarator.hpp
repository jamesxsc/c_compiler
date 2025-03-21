#pragma once

#include "ast_node.hpp"
#include "ast_declarator.hpp"
#include "ast_initializer.hpp"

namespace ast {

    class InitDeclarator : public Node {
    public:
        InitDeclarator(DeclaratorPtr declarator, InitializerPtr initializer) : declarator_(std::move(declarator)),
                                                                               initializer_(std::move(initializer)) {}

        explicit InitDeclarator(DeclaratorPtr declarator) : declarator_(std::move(declarator)), initializer_(nullptr) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] const std::string &GetIdentifier() const;

        [[nodiscard]] bool HasInitializer() const;

        [[nodiscard]] const Initializer &GetInitializer() const;

        [[nodiscard]] bool IsPointer() const;

        [[nodiscard]] Function BuildFunction(TypeSpecifier returnType, Context &context) const; // Pass down to declarator

        [[nodiscard]] Variable BuildArray(TypeSpecifier type, Context &context) const; // Pass down to declarator

        [[nodiscard]] bool IsFunction() const;

        [[nodiscard]] bool IsArray() const;

        [[nodiscard]] std::string GetGlobalInitializerIdentifier() const;

        template<class T>
        [[nodiscard]] T EvaluateInitializer(Context &context) const;

    private:
        DeclaratorPtr declarator_;
        InitializerPtr initializer_;
    };

    using InitDeclaratorPtr = std::unique_ptr<const InitDeclarator>;

}
