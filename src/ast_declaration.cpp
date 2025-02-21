#include "ast_declaration.hpp"

namespace ast {

    void Declaration::EmitRISC(std::ostream &stream, Context &context, int destReg) const {

    }

    void Declaration::Print(std::ostream &stream) const {
        stream << type_ << " ";
        identifier_->Print(stream);
    }

    const NodePtr &Declaration::GetIdentifier() const {
        return identifier_;
    }

} // namespace ast
