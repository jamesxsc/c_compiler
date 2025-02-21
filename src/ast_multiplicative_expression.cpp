#include "ast_multiplicative_expression.hpp"

namespace ast {

    void MultiplicativeExpression::EmitRISC(std::ostream &stream, Context &context, int destReg) const {
        // TODO a better type for destReg/ registers in general?
        switch (op_) {
            case MultiplicativeOperator::Multiply: {
                int leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);
                int rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);
                stream << "mul x" << destReg << " t" << leftReg << " t" << rightReg << std::endl;
                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }
            case MultiplicativeOperator::UnaryPromote: {
                left_->EmitRISC(stream, context, destReg);
                break;
            }
            // TODO remaining operators
            case MultiplicativeOperator::Divide: {
                std::cerr << "Divide not implemented" << std::endl;
                break;
            }
            case MultiplicativeOperator::Modulo: {
                std::cerr << "Modulo not implemented" << std::endl;
                break;
            }
        }

    }

    void MultiplicativeExpression::Print(std::ostream &stream) const {
        left_->Print(stream);
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
        if (right_ != nullptr)
            right_->Print(stream);
    }

} // namespace ast
