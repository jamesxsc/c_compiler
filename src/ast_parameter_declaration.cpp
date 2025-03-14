#include <cassert>
#include "ast_parameter_declaration.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void ParameterDeclaration::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
    }

    void ParameterDeclaration::Print(std::ostream &stream) const {
        declarationSpecifiers_->Print(stream);
        stream << identifier_->GetIdentifier();
    }

    const std::string &ParameterDeclaration::GetIdentifier() const {
        return identifier_->GetIdentifier();
    }

    TypeSpecifier ParameterDeclaration::GetType(Context& context) const {
        TypeSpecifier type = declarationSpecifiers_->GetType(context);
        if (identifier_->IsPointer())
            type = {TypeSpecifier::POINTER, type};
        if (identifier_->IsArray())
            type = identifier_->BuildArray(type, context).type;
        return type;
    }

} // namespace ast
