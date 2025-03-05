#include "ast_exclusive_or_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void ExclusiveOrExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (left_ == nullptr) { // Promotion from and expression
            right_->EmitRISC(stream, context, destReg);
        } else {
            Register leftReg = context.AllocateTemporary();
            left_->EmitRISC(stream, context, leftReg);
            Register rightReg = context.AllocateTemporary();
            right_->EmitRISC(stream, context, rightReg);
            stream << "xor " << destReg << "," << leftReg << "," << rightReg << std::endl;
            context.FreeTemporary(leftReg);
            context.FreeTemporary(rightReg);
        }
    }

    void ExclusiveOrExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr) {
            left_->Print(stream);
            stream << " ^ ";
        }
        right_->Print(stream);

    }


    TypeSpecifier ExclusiveOrExpression::GetType(Context& context) const {
        return right_->GetType(context);
    }

    bool ExclusiveOrExpression::ContainsFunctionCall() const {
        return (left_ != nullptr && left_->ContainsFunctionCall()) || right_->ContainsFunctionCall();
    }

}
