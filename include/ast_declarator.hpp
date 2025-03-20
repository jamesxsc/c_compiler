#pragma once

#include "ast_node.hpp"
#include "ast_constant_expression.hpp"
#include "ast_initializer.hpp"

namespace ast {

    class Declarator;

    using DeclaratorPtr = std::unique_ptr<const Declarator>;
    class Declarator : public Node {
    protected:
        std::string identifier_;
        bool isDirect_;
        bool isPointer_;
        int indirectionLevel_ = 0;
        bool isArray_{false};
        int arrayDimension_ = 0;
        std::vector<ConstantExpressionPtr> arraySizes_{};

    public:
        explicit Declarator(std::string identifier, bool isDirect) : identifier_(std::move(identifier)),
                                                                     isDirect_(isDirect),
                                                                     isPointer_(false) {};

        Declarator(Declarator&&) = default;

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;

        void Print(std::ostream &stream) const override;

        [[nodiscard]] const std::string &GetIdentifier() const;

        [[nodiscard]] bool IsPointer() const;

        void SetPointer(int indirectionLevel);

        [[nodiscard]] virtual bool IsFunction() const;

        [[nodiscard]] virtual bool IsArray() const;

        void AddArrayDimension(ConstantExpressionPtr size);

        virtual void SetPointerReturn(int indirectionLevel);

        [[nodiscard]] virtual Function BuildFunction(TypeSpecifier returnType, Context &context) const;

        [[nodiscard]] Variable BuildArray(TypeSpecifier type, Context &context, const Initializer *initializer = nullptr) const;

        [[nodiscard]] bool IsDirect() const;

        void Indirect();

        void Direct();

    };

}
