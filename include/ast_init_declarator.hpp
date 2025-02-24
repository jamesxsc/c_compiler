#pragma once

#include "ast_node.hpp"
#include "ast_declarator.hpp"
#include "ast_initializer.hpp"

namespace ast {

    class InitDeclarator : public Node {
    public:
        InitDeclarator(DeclaratorPtr declarator, InitializerPtr initializer) : declarator_(std::move(declarator)), initializer_(std::move(initializer)) {}
        InitDeclarator(DeclaratorPtr declarator) : declarator_(std::move(declarator)), initializer_(nullptr) {}

        void EmitRISC(std::ostream &stream, Context &context, Register destReg) const override;
        void Print(std::ostream &stream) const override;
    private:
        DeclaratorPtr declarator_;
        InitializerPtr initializer_;
    };

    using InitDeclaratorPtr = std::unique_ptr<const InitDeclarator>;

}
