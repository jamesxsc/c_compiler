#include "ast_unary_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void UnaryExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        child_->EmitRISC(stream, context, destReg);
    }

    void UnaryExpression::Print(std::ostream &stream) const {
        child_->Print(stream);
    }

    Type UnaryExpression::GetType(Context &context) const { 
        return child_->GetType(context);
    }

}
