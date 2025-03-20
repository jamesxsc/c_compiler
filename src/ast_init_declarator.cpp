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

    Variable InitDeclarator::BuildArray(TypeSpecifier type, Context &context) const {
        return declarator_->BuildArray(type, context, initializer_.get());
    }

    bool InitDeclarator::IsArray() const {
        return declarator_->IsArray();
    }

    std::string InitDeclarator::GetGlobalInitializerIdentifier() const {
        return initializer_->GetGlobalIdentifier();
    }

    template<class T>
    T InitDeclarator::EvaluateInitializer(Context &context) const {
        throw std::runtime_error("InitDeclarator::EvaluateInitializer() called on an unsupported type");
    }

    template<>
    int InitDeclarator::EvaluateInitializer<int>(Context &context) const {
        return initializer_->Evaluate<int>(context);
    }

    template<>
    double InitDeclarator::EvaluateInitializer<double>(Context &context) const {
        return initializer_->Evaluate<double>(context);
    }

    const Initializer &InitDeclarator::GetInitializer() const {
        return *initializer_;
    }

}
