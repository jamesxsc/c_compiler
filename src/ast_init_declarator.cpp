#include "ast_init_declarator.hpp"
#include "ast_function_declarator.hpp"

namespace ast {

    void InitDeclarator::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (initializer_) {
            initializer_->EmitRISC(stream, context, destReg);
        }
    }

    void InitDeclarator::Print(std::ostream &stream) const {
        declarator_->Print(stream);
        if (initializer_) {
            stream << " = ";
            initializer_->Print(stream);
        }
        stream << ";" << std::endl; // Note this may need to be removed if this ends up being done in statement
    }

    const std::string &InitDeclarator::GetIdentifier() const {
        return declarator_->GetIdentifier();
    }

    bool InitDeclarator::HasInitializer() const {
        return initializer_ != nullptr;
    }

    bool InitDeclarator::IsFunction() const {
        return declarator_->IsFunction();
    }

    bool InitDeclarator::IsPointer() const {
        return declarator_->IsPointer();
    }

    Function InitDeclarator::BuildFunction(TypeSpecifier returnType, Context &context) const {
        return declarator_->BuildFunction(returnType, context);
    }

    std::string InitDeclarator::GetGlobalInitializerIdentifier() const {
        return initializer_->GetGlobalIdentifier();
    }

    int InitDeclarator::GetGlobalInitializerValue() const {
        return initializer_->GetGlobalValue();
    }

}
