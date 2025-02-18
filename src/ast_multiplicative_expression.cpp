#include "ast_multiplicative_expression.hpp"

namespace ast {

    void MultiplicativeExpression::EmitRISC(std::ostream &stream, Context &context) const {
        // TODO actually use temporaries... context?
        left_->EmitRISC(stream, context);
        right_->EmitRISC(stream, context);
        stream << "mul" << std::endl;
    }

    void MultiplicativeExpression::Print(std::ostream &stream) const {
        left_->Print(stream);
        stream << " * ";
        right_->Print(stream);
    }

} // namespace ast
