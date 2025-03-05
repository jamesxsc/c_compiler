#include "ast_expression.hpp"
#include "ast_assignment_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

Expression::~Expression() = default;

void Expression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
    if (first_)
        first_->EmitRISC(stream, context, destReg);

    assignment_->EmitRISC(stream, context, destReg);
}

void Expression::Print(std::ostream &stream) const {
    if (first_) {
        first_->Print(stream);
        stream << ", ";
    }
    assignment_->Print(stream);
}

Type Expression::GetType(Context&) const {
    return Type(TypeSpecifier::INT, true);
}

} // namespace ast
