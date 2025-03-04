#pragma once

#include <string>
#include <memory>
#include "ast_node.hpp"
#include "ast_initializer.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    class Declarator : public Node {
    private:
        std::string identifier_;
        bool isDirect_;
        TypeSpecifier typeSpecifier_;
        bool hasInitializer_;
        Initializer* initializer_;

    public:
        // Only the constructor declaration here (no inline definition).
        explicit Declarator(std::string identifier, bool isDirect);

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;

        [[nodiscard]] const std::string& GetIdentifier() const;
        [[nodiscard]] bool IsDirect() const;

        void Indirect();
        void Direct();

        Type GetType(Context &context) const;

        void SetTypeSpecifier(TypeSpecifier spec);
        TypeSpecifier GetTypeSpecifier() const;

        void SetHasInitializer(bool h);
        bool HasInitializer() const;

        void SetInitializer(Initializer* init);
        Initializer* GetInitializer() const;

        bool IsFunction() const;
    };

    using DeclaratorPtr = std::unique_ptr<const Declarator>;

} // namespace ast
