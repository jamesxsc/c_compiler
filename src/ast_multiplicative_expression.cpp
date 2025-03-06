#include <cassert>
#include "ast_multiplicative_unary_expressions.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void MultiplicativeExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (op_ == MultiplicativeOperator::UnaryPromote) {
            right_->EmitRISC(stream, context, destReg);
            return;
        }

        // Common for all ops
        bool leftStored = right_->ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent() : context.AllocateTemporary();
        left_->EmitRISC(stream, context, leftReg);
        Register rightReg = context.AllocateTemporary();
        right_->EmitRISC(stream, context, rightReg);

        switch (op_) {
            case MultiplicativeOperator::Multiply:
                stream << "mul " << destReg << "," << leftReg << "," << rightReg << std::endl;
                break;
            case MultiplicativeOperator::Divide:
                stream << "div " << destReg << "," << leftReg << "," << rightReg << std::endl;
                break;
            case MultiplicativeOperator::Modulo:
                if (IsSigned(left_->GetType(context)) && IsSigned(right_->GetType(context)))
                    stream << "rem " << destReg << "," << leftReg << "," << rightReg << std::endl;
                else
                    stream << "remu " << destReg << "," << leftReg << "," << rightReg << std::endl;
                break;
            case MultiplicativeOperator::UnaryPromote: // Should never reach here
                break;
        }

        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg);
        context.FreeTemporary(rightReg);
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
        return right_->GetType(context);
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
}
