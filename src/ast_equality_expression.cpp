#include "ast_equality_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void EqualityExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (op_ == EqualityOperator::RelationalPromote) {
            right_->EmitRISC(stream, context, destReg);
            return;
        }

        bool leftStored = right_->ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent() : context.AllocateTemporary();
        left_->EmitRISC(stream, context, leftReg);
        Register rightReg = context.AllocateTemporary();
        right_->EmitRISC(stream, context, rightReg);
        switch (op_) {
            case EqualityOperator::Equality:
                stream << "sub " << destReg << "," << leftReg << "," << rightReg << std::endl;
                stream << "seqz " << destReg << "," << destReg << std::endl;
                stream << "andi " << destReg << "," << destReg << ",0xff" << std::endl;
                break;
            case EqualityOperator::Inequality:
                stream << "sub " << destReg << "," << leftReg << "," << rightReg << std::endl;
                stream << "snez " << destReg << "," << destReg << std::endl;
                stream << "andi " << destReg << "," << destReg << ",0xff" << std::endl;
                break;
            case EqualityOperator::RelationalPromote: // Should never reach here
                break;
        }
        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg);
        context.FreeTemporary(rightReg);
    }

    void EqualityExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr) {
            left_->Print(stream);
        }
        switch (op_) {
            case EqualityOperator::Equality: {
                stream << " == ";
                break;
            }
            case EqualityOperator::Inequality: {
                stream << " != ";
                break;
            }
            case EqualityOperator::RelationalPromote: {
                break;
            }
        }
        right_->Print(stream);
    }


    TypeSpecifier EqualityExpression::GetType(Context & context) const {
        return right_->GetType(context);
    }

    bool EqualityExpression::ContainsFunctionCall() const {
        if (op_ == EqualityOperator::RelationalPromote) {
            return right_->ContainsFunctionCall();
        } else {
            return left_->ContainsFunctionCall() || right_->ContainsFunctionCall();
        }
    }

}