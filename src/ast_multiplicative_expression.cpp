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
        TypeSpecifier type = GetType(context);
        bool useFloat = type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE;
        bool leftStored = right_->ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent(useFloat) : context.AllocateTemporary(useFloat);
        left_->EmitRISC(stream, context, leftReg);
        Register rightReg = context.AllocateTemporary(useFloat);
        right_->EmitRISC(stream, context, rightReg);

        switch (type) {
            case TypeSpecifier::FLOAT:
            case TypeSpecifier::DOUBLE:
                switch (op_) {
                    case MultiplicativeOperator::Multiply:
                        stream << (type == TypeSpecifier::FLOAT ? "fmul.s " : "fmul.d ") << destReg << "," << leftReg << "," << rightReg << std::endl;
                        break;
                    case MultiplicativeOperator::Divide:
                        stream << (type == TypeSpecifier::FLOAT ? "fdiv.s " : "fdiv.d ") << destReg << "," << leftReg << "," << rightReg << std::endl;
                        break;
                    case MultiplicativeOperator::Modulo:
                        throw std::runtime_error("Modulo operator not supported for floating point types");
                    case MultiplicativeOperator::UnaryPromote: // Should never reach here
                        break;
                }
                break;
            case TypeSpecifier::INT:
                switch (op_) {
                    case MultiplicativeOperator::Multiply:
                        stream << "mul " << destReg << "," << leftReg << "," << rightReg << std::endl;
                        break;
                    case MultiplicativeOperator::Divide:
                        stream << "div " << destReg << "," << leftReg << "," << rightReg << std::endl;
                        break;
                    case MultiplicativeOperator::Modulo:
                        if (left_->GetType(context).IsSigned() && right_->GetType(context).IsSigned()) // todo these kind of expressions could probably be simplified into a case: UNSIGNED and set that in GetType on this class
                            stream << "rem " << destReg << "," << leftReg << "," << rightReg << std::endl;
                        else
                            stream << "remu " << destReg << "," << leftReg << "," << rightReg << std::endl;
                        break;
                    case MultiplicativeOperator::UnaryPromote: // Should never reach here
                        break;
                }
                break;
            default:
                throw std::runtime_error("Multiplicative operation attempted on unsupported type.");
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

    // These are constants which will always simply be a UnaryPromote
    int MultiplicativeExpression::GetGlobalValue() const {
        return right_->GetGlobalValue();
    }

    std::string MultiplicativeExpression::GetGlobalIdentifier() const {
        return right_->GetGlobalIdentifier();
    }

}
