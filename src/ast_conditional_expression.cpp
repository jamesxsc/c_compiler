#include "ast_conditional_expression.hpp"
#include "ast_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void ConditionalExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // TODO implement
        // Do we reuse some logic from if/else blocks?
        // for now (temp) just emit the middle
        if (ternary_)
            std::cerr << ":ConditionalExpression::EmitRISC Temporary logic" << std::endl;
        left_->EmitRISC(stream, context, destReg);
    }

    void ConditionalExpression::Print(std::ostream &stream) const {
        if (ternary_) {
            left_->Print(stream);
            stream << " ? ";
            middle_->Print(stream);
            stream << " : ";
            right_->Print(stream);
        } else {
            left_->Print(stream);
        }
    }

    ConditionalExpression::ConditionalExpression(LogicalOrExpressionPtr left, ExpressionPtr middle,
                                                 ConditionalExpressionPtr right) : ternary_(true), left_(std::move(left)), middle_(std::move(middle)), right_(std::move(right)) {}

    ConditionalExpression::ConditionalExpression(LogicalOrExpressionPtr left) : ternary_(false), left_(std::move(left)), middle_(nullptr), right_(
            nullptr) {}



    ast::Type ConditionalExpression::GetType(Context&) const {
        return ast::Type(ast::TypeSpecifier::INT, true);
    }

    bool ConditionalExpression::ContainsFunctionCall() const {
        if (ternary_) {
            return left_->ContainsFunctionCall() || middle_->ContainsFunctionCall() || right_->ContainsFunctionCall();
        } else {
            return left_->ContainsFunctionCall();
        }
    }

}
