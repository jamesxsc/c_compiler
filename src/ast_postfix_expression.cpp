#include "ast_postfix_expression.hpp"
#include "ast_identifier.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void PostfixExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        switch (op_) {
            case PostfixOperator::PrimaryPromote:
            case PostfixOperator::FunctionCallPromote:
            case PostfixOperator::ArrayIndexPromote:
                child_->EmitRISC(stream, context, destReg);
                break;
            case PostfixOperator::PostfixIncrement:
            case PostfixOperator::PostfixDecrement:
                std::string identifier = GetIdentifier();
                if (context.IsGlobal(identifier)) {
                    // Note this is slightly different from GCC because I want to delegate to child
                    // Store the incremented/decremented value in the destination register
                    if (destReg != Register::zero)
                        child_->EmitRISC(stream, context, destReg);
                    Register tempReg = context.AllocateTemporary();
                    Register tempReg2 = context.AllocateTemporary();
                    stream << "lui " << tempReg2 << ",%hi(" << identifier << ")" << std::endl;
                    stream << "lw " << tempReg2 << ",%lo(" << identifier << ")(" << tempReg2 << ")" << std::endl;
                    stream << "addi " << tempReg << "," << tempReg2 << ","
                           << (op_ == PostfixOperator::PostfixIncrement ? "1" : "-1") << std::endl;
                    stream << "lui " << tempReg2 << ",%hi(" << identifier << ")" << std::endl;
                    stream << "sw " << tempReg << ",%lo(" << identifier << ")(" << tempReg2 << ")" << std::endl;
                    context.FreeTemporary(tempReg);
                    context.FreeTemporary(tempReg2);
                } else {
                    Variable variable = context.CurrentFrame().bindings.Get(GetIdentifier());
                    // Store the pre-inc/dec value in the destination register
                    if (destReg != Register::zero)
                        child_->EmitRISC(stream, context, destReg);
                    Register tempReg = context.AllocateTemporary();
                    stream << "lw " << tempReg << "," << variable.offset << "(s0)" << std::endl;
                    stream << "addi " << tempReg << "," << tempReg << ","
                           << (op_ == PostfixOperator::PostfixIncrement ? 1 : -1) << std::endl;
                    stream << "sw " << tempReg << "," << variable.offset << "(s0)" << std::endl;
                    context.FreeTemporary(tempReg);
                }
                break;
        }
    }

    void PostfixExpression::Print(std::ostream &stream) const {
        switch (op_) {
            case PostfixOperator::PrimaryPromote:
            case PostfixOperator::FunctionCallPromote:
            case PostfixOperator::ArrayIndexPromote:
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

    TypeSpecifier PostfixExpression::GetType(Context &context) const {
        return child_->GetType(context);
    }

    std::string PostfixExpression::GetIdentifier() const {
        // todo handle array/identifier.member case complicated - cant just get from bindings

        switch (op_) {
            case PostfixOperator::PrimaryPromote:
            case PostfixOperator::FunctionCallPromote: {
                Identifier identifier = dynamic_cast<const Identifier &>(*child_);
                return identifier.GetIdentifier();
            }
            case PostfixOperator::PostfixIncrement:
            case PostfixOperator::PostfixDecrement: {
                // Child is a postfix expression
                return dynamic_cast<const PostfixExpression *>(child_.get())->GetIdentifier();
            }
            case PostfixOperator::ArrayIndexPromote:
                throw std::runtime_error("Array index identifier required");
                break;
        }

        Identifier identifier = dynamic_cast<const Identifier &>(*child_);
        return identifier.GetIdentifier();
    }

    bool PostfixExpression::ContainsFunctionCall() const {
        return child_->ContainsFunctionCall();
    }

    int PostfixExpression::GetGlobalValue() const {
        return child_->GetGlobalValue();
    }

    std::string PostfixExpression::GetGlobalIdentifier() const {
        return child_->GetGlobalIdentifier();
    }

} // namespace ast
