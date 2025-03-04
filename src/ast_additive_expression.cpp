#include "ast_type_specifier.hpp"
#include "ast_additive_expression.hpp"

namespace ast {

void AdditiveExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const
{
    Type lt = left_->GetType(context);
    Type rt = right_->GetType(context);
    bool isFloat = lt.GetSpecifier() == TypeSpecifier::FLOAT || rt.GetSpecifier() == TypeSpecifier::FLOAT;

    switch (op_) {
        case AdditiveOperator::Add: {
            Register leftReg = context.AllocateTemporary();
            left_->EmitRISC(stream, context, leftReg);
            Register rightReg = context.AllocateTemporary();
            right_->EmitRISC(stream, context, rightReg);
            if (isFloat) {
                stream << "fadd.s " << destReg << "," << leftReg << "," << rightReg << std::endl;
            } else {
                stream << "add " << destReg << "," << leftReg << "," << rightReg << std::endl;
            }
            context.FreeTemporary(leftReg);
            context.FreeTemporary(rightReg);
            break;
        }
        case AdditiveOperator::Subtract: {
            Register leftReg = context.AllocateTemporary();
            left_->EmitRISC(stream, context, leftReg);
            Register rightReg = context.AllocateTemporary();
            right_->EmitRISC(stream, context, rightReg);
            if (isFloat) {
                stream << "fsub.s " << destReg << "," << leftReg << "," << rightReg << std::endl;
            } else {
                stream << "sub " << destReg << "," << leftReg << "," << rightReg << std::endl;
            }
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

Type AdditiveExpression::GetType(Context& context) const {
    Type lt = left_->GetType(context);
    Type rt = right_->GetType(context);
    if (lt.GetSpecifier() == TypeSpecifier::FLOAT || rt.GetSpecifier() == TypeSpecifier::FLOAT) {
        return Type(TypeSpecifier::FLOAT, true);
    }
    return Type(TypeSpecifier::INT, true);
}

}
