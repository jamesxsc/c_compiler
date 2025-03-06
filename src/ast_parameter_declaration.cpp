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

    TypeSpecifier ParameterDeclaration::GetType(Context&) const {
        assert(!declarationSpecifiers_->GetTypeSpecifiers().empty() && "Parameter declaration must have type specifier");
        return declarationSpecifiers_->GetTypeSpecifiers().front();
    }

} // namespace ast
