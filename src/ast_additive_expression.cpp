#include "ast_type_specifier.hpp"
#include "ast_additive_expression.hpp"

namespace ast {

    // todo nice to have : extract a binaryexpression class to handle common logic
    void AdditiveExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (op_ == AdditiveOperator::MultiplicativePromote) {
            right_->EmitRISC(stream, context, destReg);
            return;
        }

        bool leftStored = right_->ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent() : context.AllocateTemporary();
        left_->EmitRISC(stream, context, leftReg);
        Register rightReg = context.AllocateTemporary();
        right_->EmitRISC(stream, context, rightReg);
        switch (op_) {
            case AdditiveOperator::Add:
                stream << "add " << destReg << "," << leftReg << "," << rightReg << std::endl;
                break;
            case AdditiveOperator::Subtract:
                stream << "sub " << destReg << "," << leftReg << "," << rightReg << std::endl;
                break;
            case AdditiveOperator::MultiplicativePromote: // Should never reach here
                break;
        }
        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg);
        context.FreeTemporary(rightReg);
    }

    void AdditiveExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr) {
            left_->Print(stream);
        }

        switch (op_) {
            case AdditiveOperator::Add:
                stream << " + ";
                break;
            case AdditiveOperator::Subtract:
                stream << " - ";
                break;
            case AdditiveOperator::MultiplicativePromote:
                break;
        }

        right_->Print(stream);
    }


    TypeSpecifier AdditiveExpression::GetType(Context &context) const {
        // todo technically should be l or r e.g. for float, need some ordering/ranking of types and propagate elsewhere
        return right_->GetType(context);
    }

    bool AdditiveExpression::ContainsFunctionCall() const {
        if (op_ == AdditiveOperator::MultiplicativePromote) {
            return right_->ContainsFunctionCall();
        } else {
            return (left_ != nullptr && left_->ContainsFunctionCall()) || right_->ContainsFunctionCall();
        }
    }

} // namespace ast
