#include "ast_aggregate_type_definition.hpp"

namespace ast {

    void AggregateTypeDefinition::EmitRISC(std::ostream &stream, ast::Context &context, ast::Register destReg) const {
        if (enumSpecifier_)
            enumSpecifier_->EmitRISC(stream, context, destReg);
    }

    void AggregateTypeDefinition::Print(std::ostream &stream) const {
        if (enumSpecifier_)
            enumSpecifier_->Print(stream);
        stream << ";" << std::endl;
    }

    bool AggregateTypeDefinition::IsAggregateTypeDefinition() const {
        return true;
    }

    void AggregateTypeDefinition::SetGlobal() {
        isGlobal_ = true;
        if (enumSpecifier_) {
            enumSpecifier_->SetGlobal();
        }
    }

}
