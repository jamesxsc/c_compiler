#include "ast_array_declarator.hpp"

namespace ast {

    // TODO implement stubs
    void ArrayDeclarator::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {

    }

    void ArrayDeclarator::Print(std::ostream &stream) const {
        stream << GetIdentifier();
        stream << "[";
        size_->Print(stream);
        stream << "]";
    }

    bool ArrayDeclarator::IsArray() const {
        return true;
    }

    Array ArrayDeclarator::BuildArray(TypeSpecifier type, Context &context) const {
        return {type, size_->GetGlobalValue()}; // Get the size like it's a global integral init
    }

}
