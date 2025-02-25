#include "ast_expression.hpp"
#include "ast_type_specifier.hpp"

// Note that this feature isn't technically required
namespace ast {

    void ast::Expression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (first_)
            first_->EmitRISC(stream, context, destReg);

        assignment_->EmitRISC(stream, context, destReg);
    }

    void ast::Expression::Print(std::ostream &stream) const {
        if (first_) {
            first_->Print(stream);
            stream << ", ";
        }
        assignment_->Print(stream);
    }



    ast::Type Expression::GetType(Context&) const {
        return ast::Type(ast::TypeSpecifier::INT, true);
    }
}
