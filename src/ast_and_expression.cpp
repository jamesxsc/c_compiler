#include "ast_and_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void AndExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (left_ == nullptr) { // Promotion from equality expression
            right_->EmitRISC(stream, context, destReg);
        } else {
            Register leftReg = context.AllocateTemporary();
            left_->EmitRISC(stream, context, leftReg);
            Register rightReg = context.AllocateTemporary();
            right_->EmitRISC(stream, context, rightReg);
            stream << "and " << destReg << "," << leftReg << "," << rightReg << std::endl;
            context.FreeTemporary(leftReg);
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

    ast::Type AndExpression::GetType(Context&) const {
        return ast::Type(ast::TypeSpecifier::INT, true);
    }

    bool AndExpression::ContainsFunctionCall() const {
        return (left_ != nullptr && left_->ContainsFunctionCall()) || right_->ContainsFunctionCall();
    }
}
