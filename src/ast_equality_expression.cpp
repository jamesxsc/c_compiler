#include "ast_equality_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void EqualityExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        switch (op_) {
            case EqualityOperator::Equality: {
                Register leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);
                Register rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);
                stream << "sub " << destReg << "," << leftReg << "," << rightReg << std::endl;
                stream << "seqz " << destReg << "," << destReg << std::endl;
                stream << "andi " << destReg << "," << destReg << ",0xff" << std::endl;
                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }
            case EqualityOperator::Inequality: {
                Register leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);
                Register rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);
                stream << "sub " << destReg << "," << leftReg << "," << rightReg << std::endl;
                stream << "snez " << destReg << "," << destReg << std::endl;
                stream << "andi " << destReg << "," << destReg << ",0xff" << std::endl;
                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }
            case EqualityOperator::RelationalPromote: {
                right_->EmitRISC(stream, context, destReg);
                break;
            }
        }
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



    ast::Type EqualityExpression::GetType(Context&) const {
        return ast::Type(ast::TypeSpecifier::INT, true);
    }
}