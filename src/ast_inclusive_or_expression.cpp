#include "ast_inclusive_or_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void InclusiveOrExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (left_ == nullptr) { // Promotion from exclusive or expression
            right_->EmitRISC(stream, context, destReg);
        } else {
            Register leftReg = context.AllocateTemporary();
            left_->EmitRISC(stream, context, leftReg);
            Register rightReg = context.AllocateTemporary();
            right_->EmitRISC(stream, context, rightReg);
            stream << "or " << destReg << "," << leftReg << "," << rightReg << std::endl;
            context.FreeTemporary(leftReg);
            context.FreeTemporary(rightReg);
        }
    }

    void InclusiveOrExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr) {
            left_->Print(stream);
            stream << " | ";
        }
        right_->Print(stream);

    }

    TypeSpecifier InclusiveOrExpression::GetType(Context& context) const {
        return right_->GetType(context);
    }

    bool InclusiveOrExpression::ContainsFunctionCall() const {
        return (left_ != nullptr && left_->ContainsFunctionCall()) || right_->ContainsFunctionCall();
    }
}
