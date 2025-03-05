#include "ast_parameter_declaration.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void ParameterDeclaration::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
    }

    void ParameterDeclaration::Print(std::ostream &stream) const {
        stream << type_ << " " << identifier_->GetIdentifier();
    }

    const std::string &ParameterDeclaration::GetIdentifier() const {
        return identifier_->GetIdentifier();
    }

    TypeSpecifier ParameterDeclaration::GetType(Context&) const {
        return type_;
    }

} // namespace ast
