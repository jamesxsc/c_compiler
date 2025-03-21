#include "ast_expression_statement.hpp"
#include <iostream>

namespace ast {

    void ExpressionStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (expression_) {
            expression_->EmitRISC(stream, context, destReg);
        }
    }

    void ExpressionStatement::Print(std::ostream &stream) const {
        if (expression_) {
            expression_->Print(stream);
        }
        stream << ";" << std::endl;
    }

    const Expression *ExpressionStatement::GetExpression() const {
        return expression_.get();
    }

} // namespace ast
