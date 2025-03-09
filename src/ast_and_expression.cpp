#include "ast_and_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void AndExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (left_ == nullptr) { // Promotion from equality expression
            right_->EmitRISC(stream, context, destReg);
        } else {
            bool leftStored = right_->ContainsFunctionCall();
            Register leftReg = leftStored ? context.AllocatePersistent() : context.AllocateTemporary();
            left_->EmitRISC(stream, context, leftReg);
            Register rightReg = context.AllocateTemporary();
            right_->EmitRISC(stream, context, rightReg);
            stream << "and " << destReg << "," << leftReg << "," << rightReg << std::endl;
            leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg);
            context.FreeTemporary(rightReg);
        }
    }

    void AndExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr) {
            left_->Print(stream);
            stream << " & ";
        }
        right_->Print(stream);
    }

    TypeSpecifier AndExpression::GetType(Context& context) const {
        return right_->GetType(context);
    }

    bool AndExpression::ContainsFunctionCall() const {
        return (left_ != nullptr && left_->ContainsFunctionCall()) || right_->ContainsFunctionCall();
    }

    std::string AndExpression::GetGlobalIdentifier() const {
        return right_->GetGlobalIdentifier();
    }

    int AndExpression::GetGlobalValue() const {
        return right_->GetGlobalValue();
    }

}
