#include "ast_type_name.hpp"

namespace ast {

    void TypeName::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        specifierQualifierList_->EmitRISC(stream, context, destReg);
    }

    void TypeName::Print(std::ostream &stream) const {
        specifierQualifierList_->Print(stream);
    }

    TypeSpecifier TypeName::GetType(Context &context) const {
        return specifierQualifierList_->GetType(context);
    }

}
