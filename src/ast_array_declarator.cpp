#include "ast_array_declarator.hpp"

namespace ast {

    void ArrayDeclarator::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Arrays are handled in declaration/external declaration
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

    Variable ArrayDeclarator::BuildArray(TypeSpecifier type, Context &context) const {
        TypeSpecifier arrayType = {type, size_->Evaluate()};
        return {
                .size = arrayType.GetTypeSize(),
                .type = arrayType,
        };
    }

}
