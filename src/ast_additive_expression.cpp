#include "ast_type_specifier.hpp"
#include "ast_additive_expression.hpp"

namespace ast {

    void AdditiveExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const
    {
        switch (op_) {
            case AdditiveOperator::Add: {
                Register leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);

                // CHECK: Here we only need to check that right doesn't clobber left?
                // Doesn't really fit the model of looking for def and use though

                Register rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);

                stream << "add " << destReg << "," << leftReg << "," << rightReg << std::endl;

                context.FreeTemporary(leftReg);
                context.FreeTemporary(rightReg);
                break;
            }

            case AdditiveOperator::Subtract: {
                Register leftReg = context.AllocateTemporary();
                left_->EmitRISC(stream, context, leftReg);

                Register rightReg = context.AllocateTemporary();
                right_->EmitRISC(stream, context, rightReg);

                stream << "sub " << destReg << "," << leftReg << "," << rightReg << std::endl;

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


    ast::Type AdditiveExpression::GetType(Context&) const {
        return ast::Type(ast::TypeSpecifier::INT, true);
    }

} // namespace ast
