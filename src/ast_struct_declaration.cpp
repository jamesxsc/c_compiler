#include "ast_struct_declaration.hpp"

namespace ast {

    void ast::StructDeclaration::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Handled above this layer
    }

    void ast::StructDeclaration::Print(std::ostream &stream) const {
        specifierQualifierList_->Print(stream);
        stream << " ";
        structDeclaratorList_->Print(stream);
        stream << ";" << std::endl;
    }

    TypeSpecifier ast::StructDeclaration::GetType(Context &context) const {
        return specifierQualifierList_->GetType(context);
    }

    const std::vector<StructDeclaratorPtr> &StructDeclaration::GetDeclarators() const {
        return structDeclaratorList_->GetDeclarators();
    }
}
