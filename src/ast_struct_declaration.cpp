#include "ast_struct_declaration.hpp"

namespace ast {

    void StructDeclaration::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Handled above this layer
    }

    void StructDeclaration::Print(std::ostream &stream) const {
        specifierQualifierList_->Print(stream);
        stream << " ";
        structDeclaratorList_->Print(stream);
        stream << ";" << std::endl;
    }

    TypeSpecifier StructDeclaration::GetType(Context &context) const {
        return specifierQualifierList_->GetType(context);
    }

    TypeSpecifier StructDeclaration::GetTypeStatic() const {
        return Context::ResolveTypeAliasStatic(specifierQualifierList_->GetSpecifiers());
    }

    const std::vector<StructDeclaratorPtr> &StructDeclaration::GetDeclarators() const {
        return structDeclaratorList_->GetDeclarators();
    }
}
