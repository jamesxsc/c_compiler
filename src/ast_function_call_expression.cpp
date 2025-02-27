#include "ast_function_call_expression.hpp"

namespace ast {

    void FunctionCallExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (arguments_ != nullptr) {
            arguments_->EmitRISC(stream, context, destReg);
        }
        stream << "call " << function_->GetIdentifier() << std::endl;
    }

    void FunctionCallExpression::Print(std::ostream &stream) const {
        stream << function_->GetIdentifier() << "(";
        if (arguments_ != nullptr) {
            arguments_->Print(stream);
        }
        stream << ")";
    }

    Type FunctionCallExpression::GetType(Context &context) const {
        return Type(TypeSpecifier::INT);
    }

}
