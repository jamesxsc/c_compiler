#include "ast_parameter_declaration.hpp"

namespace ast {

    void ParameterDeclaration::EmitRISC(std::ostream &stream, Context &context, int destReg) const {

    }

    void ParameterDeclaration::Print(std::ostream &stream) const {
        stream << type_ << " ";
        identifier_->Print(stream);
    }

    const NodePtr &ParameterDeclaration::GetIdentifier() const {
        return identifier_;
    }

} // namespace ast
