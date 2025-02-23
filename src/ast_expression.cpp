#include "ast_expression.hpp"

namespace ast {

    void ast::Expression::EmitRISC(std::ostream &stream, Context &context, int destReg) const {
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

}
