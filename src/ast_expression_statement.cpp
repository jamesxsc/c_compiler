#include "ast_expression_statement.hpp"

namespace ast {

    void ExpressionStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        expression_->EmitRISC(stream, context, destReg);
    }

    void ExpressionStatement::Print(std::ostream &stream) const {
        expression_->Print(stream);
        stream << ";" << std::endl;
    }

}
