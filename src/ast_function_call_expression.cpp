#include "ast_function_call_expression.hpp"

namespace ast {

    void FunctionCallExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (arguments_ != nullptr) {
            arguments_->EmitRISC(stream, context, destReg);
        }
        stream << "call " << function_->GetIdentifier() << std::endl;
        if (GetType(context) == TypeSpecifier::VOID) return;
        if (GetType(context) == TypeSpecifier::FLOAT || GetType(context) == TypeSpecifier::DOUBLE) {
            if (destReg != Register::fa0 && destReg != Register::zero)
                stream << (GetType(context) == TypeSpecifier::DOUBLE ? "fmv.d " : "fmv.s ") << destReg << "," << Register::fa0
                       << std::endl; // Assumes single return value in fa0
        } else {
            if (destReg != Register::a0 && destReg != Register::zero)
                stream << "mv " << destReg << "," << Register::a0 << std::endl; // Assumes single return value in a0
        }
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

    std::string FunctionCallExpression::GetGlobalIdentifier() const {
        throw std::runtime_error("FunctionCallExpression::GetGlobalIdentifier() called on a function call");
    }

    int FunctionCallExpression::Evaluate(Context &context) const {
        throw std::runtime_error("FunctionCallExpression::Evaluate() called on a function call");
    }

    double FunctionCallExpression::EvaluateFloat(Context &context) const {
        throw std::runtime_error("FunctionCallExpression::EvaluateFloat() called on a function call");
    }

}
