#pragma once

#include "ast_node.hpp"

namespace ast {

    class Declarator;

    using DeclaratorPtr = std::unique_ptr<const Declarator>;
    class Declarator : public Node {
    protected:
        std::string identifier_;
        bool isDirect_;
        bool isPointer_;
        int indirectionLevel_ = 0;

    public:
        explicit Declarator(std::string identifier, bool isDirect) : identifier_(std::move(identifier)),
                                                                     isDirect_(isDirect),
                                                                     isPointer_(false) {};

        // For derived classes
        explicit Declarator(DeclaratorPtr &&other)  noexcept : identifier_(other->identifier_), isDirect_(other->isDirect_),
                                              isPointer_(other->isPointer_), indirectionLevel_(other->indirectionLevel_) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] const std::string &GetIdentifier() const;

        [[nodiscard]] bool IsPointer() const;

        void SetPointer(int indirectionLevel);

        [[nodiscard]] virtual bool IsFunction() const;

        [[nodiscard]] virtual bool IsArray() const;

        virtual void SetPointerReturn(int indirectionLevel);

        [[nodiscard]] virtual Function BuildFunction(TypeSpecifier returnType, Context &context) const;

        [[nodiscard]] virtual Variable BuildArray(TypeSpecifier type, Context &context) const;

        [[nodiscard]] bool IsDirect() const;

        void Indirect();

        void Direct();

    };

}
