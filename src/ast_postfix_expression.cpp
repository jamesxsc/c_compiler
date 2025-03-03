#include "ast_postfix_expression.hpp"
#include "ast_identifier.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void PostfixExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        switch (op_) {
            case PostfixOperator::PrimaryPromote:
            case PostfixOperator::FunctionCallPromote:
                child_->EmitRISC(stream, context, destReg);
                break;
            case PostfixOperator::PostfixIncrement:
            case PostfixOperator::PostfixDecrement:
                Variable variable = context.CurrentFrame().bindings.at(GetIdentifier());
                Register tempReg = context.AllocateTemporary();
                child_->EmitRISC(stream, context, destReg);
                stream << "lw " << destReg << "," << variable.offset << "(s0)" << std::endl;
                stream << "addi " << tempReg << "," << destReg << ","
                       << (op_ == PostfixOperator::PostfixIncrement ? 1 : -1) << std::endl;
                // Store the pre-inc/dec value in the destination register
                stream << "sw " << tempReg << "," << variable.offset << "(s0)" << std::endl;
                context.FreeTemporary(tempReg);
                break;
        }
    }

    void PostfixExpression::Print(std::ostream &stream) const {
        switch (op_) {
            case PostfixOperator::PrimaryPromote:
            case PostfixOperator::FunctionCallPromote:
                child_->Print(stream);
                break;
            case PostfixOperator::PostfixIncrement:
                child_->Print(stream);
                stream << "++";
                break;
            case PostfixOperator::PostfixDecrement:
                child_->Print(stream);
                stream << "--";
                break;
        }
    }

    Type PostfixExpression::GetType(Context &context) const {
        return child_->GetType(context);
    }

    std::string PostfixExpression::GetIdentifier() const {
        // todo handle array[constexpr]/identifier.member case

        switch (op_) {
            case PostfixOperator::PrimaryPromote:
            case PostfixOperator::FunctionCallPromote: {
                Identifier identifier = dynamic_cast<const Identifier &>(*child_);
                return identifier.GetIdentifier();
            }
            case PostfixOperator::PostfixIncrement:
            case PostfixOperator::PostfixDecrement: {
                // Child is a postfix expression
                return dynamic_cast<const PostfixExpression*>(child_.get())->GetIdentifier();
            }
        }

        Identifier identifier = dynamic_cast<const Identifier &>(*child_);
        return identifier.GetIdentifier();
    }

    bool PostfixExpression::ContainsFunctionCall() const {
        return child_->ContainsFunctionCall();
    }

} // namespace ast
