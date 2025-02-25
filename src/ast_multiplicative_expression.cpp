#include "ast_multiplicative_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void MultiplicativeExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const
    {
        switch (op_) {
            case MultiplicativeOperator::Multiply: {
                Register leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);

                Register rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);

                stream << "mul " << destReg << "," << leftReg << "," << rightReg << std::endl;

                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }

            case MultiplicativeOperator::Divide: {
                Register leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);

                Register rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);

                stream << "div " << destReg << "," << leftReg << "," << rightReg << std::endl;

                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }

            case MultiplicativeOperator::Modulo: {
                Register leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);

                Register rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);

                // Signed remainder. For unsigned, use 'remu'.
                stream << "rem " << destReg << "," << leftReg << "," << rightReg << std::endl;

                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }

            case MultiplicativeOperator::UnaryPromote: {
                right_->EmitRISC(stream, context, destReg);
                break;
            }
        }
    }

    void MultiplicativeExpression::Print(std::ostream &stream) const
    {
        if (left_ != nullptr) {
            left_->Print(stream);
        }
        switch (op_) {
            case MultiplicativeOperator::Multiply:
                stream << " * ";
                break;
            case MultiplicativeOperator::Divide:
                stream << " / ";
                break;
            case MultiplicativeOperator::Modulo:
                stream << " % ";
                break;
            case MultiplicativeOperator::UnaryPromote:
                break;
        }
        right_->Print(stream);
    }



    ast::Type MultiplicativeExpression::GetType(Context&) const {
        return ast::Type(ast::TypeSpecifier::INT, true);
    }
}
