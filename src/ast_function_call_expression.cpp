#include "ast_function_call_expression.hpp"

namespace ast {

    void FunctionCallExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (arguments_ != nullptr) {
            arguments_->EmitRISC(stream, context, destReg);
        }
        stream << "call " << function_->GetIdentifier() << std::endl;
        if (destReg != Register::a0)
            stream << "mv " << destReg << "," << Register::a0 << std::endl; // Assumes single return value in a0
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
