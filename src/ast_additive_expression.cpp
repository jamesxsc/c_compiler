#include "ast_additive_expression.hpp"

namespace ast {

    void AdditiveExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const
    {
        switch (op_) {
            case AdditiveOperator::Add: {
                Register leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);

                Register rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);

                // RISC-V instruction for addition
                stream << "add " << destReg << "," << leftReg << "," << rightReg << std::endl;

                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }

            case AdditiveOperator::Subtract: {
                // Evaluate left and right subexpressions into temporary registers
                Register leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);

                Register rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);

                // RISC-V instruction for subtraction
                stream << "sub " << destReg << "," << leftReg << "," << rightReg << std::endl;

                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }

            case AdditiveOperator::MultiplicativePromote: {
                // Just emit the sub-expression directly into destReg
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
                // No operator for a promote
                break;
        }
        
        right_->Print(stream);
    }

} // namespace ast
