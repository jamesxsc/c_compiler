#include "ast_type_specifier.hpp"
#include "ast_additive_expression.hpp"

namespace ast {

    void AdditiveExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const
    {
        switch (op_) {
            case AdditiveOperator::Add: {
                bool leftStored = right_->ContainsFunctionCall();

                Register leftReg = leftStored ? context.AllocatePersistent() : context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);

                Register rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);

                stream << "add " << destReg << "," << leftReg << "," << rightReg << std::endl;

                leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }

            case AdditiveOperator::Subtract: {
                Register leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);

                Register rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);

                stream << "sub " << destReg << "," << leftReg << "," << rightReg << std::endl;

                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }

            case AdditiveOperator::MultiplicativePromote: {
                right_->EmitRISC(stream, context, destReg);
                break;
            }
        }
    }

    void AdditiveExpression::Print(std::ostream &stream) const
    {
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


    TypeSpecifier AdditiveExpression::GetType(Context& context) const {
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
