#include <cassert>
#include "ast_multiplicative_unary_expressions.hpp"
#include "ast_type_specifier.hpp"
#include "risc_utils.hpp"

namespace ast {

    void MultiplicativeExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        switch (op_) {
            case MultiplicativeOperator::UnaryPromote:
                right_->EmitRISC(stream, context, destReg);
                break;
            case MultiplicativeOperator::Multiply:
                Utils::EmitMultiply(stream, context, destReg, *left_, *right_);
                break;
            case MultiplicativeOperator::Divide:
                Utils::EmitDivide(stream, context, destReg, *left_, *right_);
                break;
            case MultiplicativeOperator::Modulo:
                Utils::EmitModulo(stream, context, destReg, *left_, *right_);
                break;
        }
    }

    void MultiplicativeExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr) {
            left_->Print(stream);
        }
        switch (op_) {
            case MultiplicativeOperator::Multiply:
                stream << " * ";
                break;
            case MultiplicativeOperator::Divide:
                stream << " / ";
                break;
            case MultiplicativeOperator::Modulo:
                stream << " % ";
                break;
            case MultiplicativeOperator::UnaryPromote:
                break;
        }
        right_->Print(stream);
    }


    TypeSpecifier MultiplicativeExpression::GetType(Context &context) const {
        if (op_ == MultiplicativeOperator::UnaryPromote) {
            if (right_->IsPointerDereference())
                return right_->GetType(context).GetPointeeType();
            else
                return right_->GetType(context);
        }

        // todo handle unary deref in here // make another method // test
        return Utils::BinaryResultType(left_->GetType(context), right_->GetType(context));
    }

    bool MultiplicativeExpression::ContainsFunctionCall() const {
        if (op_ == MultiplicativeOperator::UnaryPromote) {
            return right_->ContainsFunctionCall();
        } else {
            return (left_ != nullptr && left_->ContainsFunctionCall()) || right_->ContainsFunctionCall();
        }
    }

    std::string MultiplicativeExpression::GetIdentifier() const {
        assert(left_ == nullptr && "MultiplicativeExpression::GetIdentifier() called on an rvalue expression");
        return right_->GetIdentifier();
    }

    // These are constants which will always simply be a UnaryPromote
    int MultiplicativeExpression::GetGlobalValue() const {
        return right_->GetGlobalValue();
    }

    std::string MultiplicativeExpression::GetGlobalIdentifier() const {
        return right_->GetGlobalIdentifier();
    }

    int MultiplicativeExpression::Evaluate() const {
        if (op_ == MultiplicativeOperator::UnaryPromote) {
            return right_->Evaluate();
        } else {
            int left = left_->Evaluate();
            int right = right_->Evaluate();
            switch (op_) {
                case MultiplicativeOperator::Multiply:
                    return left * right;
                case MultiplicativeOperator::Divide:
                    return left / right;
                case MultiplicativeOperator::Modulo:
                    return left % right;
                case MultiplicativeOperator::UnaryPromote:
                    break;
            }
        }
        std::cerr << "Invalid multiplicative operator" << std::endl;
        exit(1);
    }

}
