#include "ast_conditional_expression.hpp"

#include "ast_type_specifier.hpp"
#include "ast_context.hpp"
#include "risc_utils.hpp"

namespace ast {

    void ConditionalExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (!ternary_) {
            left_->EmitRISC(stream, context, destReg);
            return;
        }

        Register condReg = context.AllocateTemporary(stream);
        Utils::EmitComparison(stream, context, condReg, *left_);
        std::string falseLabel = context.MakeLabel("ternary_false");
        std::string joinLabel = context.MakeLabel("ternary_join");
        stream << "beqz " << condReg << "," << falseLabel << std::endl;
        context.FreeTemporary(condReg, stream);
        middle_->EmitRISC(stream, context, destReg);
        stream << "j " << joinLabel << std::endl;
        stream << falseLabel << ":" << std::endl;
        right_->EmitRISC(stream, context, destReg);
        stream << joinLabel << ":" << std::endl;
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
                                                 ConditionalExpressionPtr right)
            : ternary_(true), left_(std::move(left)), middle_(std::move(middle)), right_(std::move(right)) {}

    ConditionalExpression::ConditionalExpression(LogicalOrExpressionPtr left)
            : ternary_(false), left_(std::move(left)), middle_(nullptr), right_(nullptr) {}

    TypeSpecifier ConditionalExpression::GetType(Context &context) const {
        if (!ternary_) {
            return left_->GetType(context);
        }
        // These should always be the same - if not it shouldn't be an issue
        return middle_->GetType(context);
    }

    bool ConditionalExpression::ContainsFunctionCall() const {
        if (ternary_) {
            return left_->ContainsFunctionCall() || middle_->ContainsFunctionCall() || right_->ContainsFunctionCall();
        } else {
            return left_->ContainsFunctionCall();
        }
    }

    // These are for constants which will never actually have a condition
    std::string ConditionalExpression::GetGlobalIdentifier() const {
        return left_->GetGlobalIdentifier();
    }

    int ConditionalExpression::Evaluate(Context &context) const {
        if (ternary_) {
            return left_->Evaluate(context) ? middle_->Evaluate(context) : right_->Evaluate(context);
        } else {
            return left_->Evaluate(context);
        }
    }

    double ConditionalExpression::EvaluateFloat(ast::Context &context) const {
        if (ternary_) {
            return left_->EvaluateFloat(context) ? middle_->EvaluateFloat(context) : right_->EvaluateFloat(context);
        } else {
            return left_->EvaluateFloat(context);
        }
    }

}
