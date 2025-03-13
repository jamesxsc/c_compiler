#include "ast_array_declarator.hpp"

namespace ast {

    void ArrayDeclarator::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Arrays are handled in declaration/external declaration
    }

    void ArrayDeclarator::Print(std::ostream &stream) const {
        // todo can we just stream gettype.. except that we don't have it here, think about it
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
