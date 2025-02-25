#include "ast_postfix_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void PostfixExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        child_->EmitRISC(stream, context, destReg);
    }

    void PostfixExpression::Print(std::ostream &stream) const {
        child_->Print(stream);
    }

    Type PostfixExpression::GetType(Context &context) const { 
        return child_->GetType(context);
    }

}
