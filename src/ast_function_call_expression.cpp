#include "ast_function_call_expression.hpp"

namespace ast {

    void FunctionCallExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (arguments_ != nullptr) {
            // todo cx what we need to use from function context here and fix mv below
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

    TypeSpecifier FunctionCallExpression::GetType(Context &context) const {
        Function function = context.GetFunction(function_->GetIdentifier());
        return function.returnType;
    }

    bool FunctionCallExpression::ContainsFunctionCall() const {
        return true;
    }

    int FunctionCallExpression::GetGlobalValue() const {
        throw std::runtime_error("FunctionCallExpression::GetGlobalValue() called on a function call");
    }

    std::string FunctionCallExpression::GetGlobalIdentifier() const {
        throw std::runtime_error("FunctionCallExpression::GetGlobalIdentifier() called on a function call");
    }

}
