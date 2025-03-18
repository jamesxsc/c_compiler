#include "ast_declarator.hpp"

namespace ast {

    void Declarator::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (isDirect_)
            return; // Nothing to do! This is part of a function or array
    }

    void Declarator::Print(std::ostream &stream) const {
        if (isDirect_)
            return; // Nothing to do! This is part of a function or array

        stream << identifier_;
    }

    const std::string &Declarator::GetIdentifier() const {
        return identifier_;
    }

    bool Declarator::IsDirect() const {
        return isDirect_;
    }

    void Declarator::Indirect() {
        isDirect_ = false;
    }

    void Declarator::Direct() {
        isDirect_ = true;
    }

    bool Declarator::IsPointer() const {
        return false;
    }

    bool Declarator::IsFunction() const {
        return false;
    }

    bool Declarator::IsArray() const {
        return false;
    }

    void Declarator::SetPointerReturn(int) {
        throw std::runtime_error("Declarator::SetPointerReturn() called on a non-function declarator");
    }

    Function Declarator::BuildFunction(TypeSpecifier returnType, Context &context) const {
        std::cerr << "Error: Declarator is not a function" << std::endl;
        exit(1);
    }

    Variable Declarator::BuildArray(TypeSpecifier type, Context &context) const {
        std::cerr << "Error: Declarator is not an array" << std::endl;
        exit(1);
    }

}
