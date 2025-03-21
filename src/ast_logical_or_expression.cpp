#include "ast_logical_or_expression.hpp"
#include "ast_type_specifier.hpp"
#include "risc_utils.hpp"

namespace ast {

    void LogicalOrExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (left_ == nullptr) { // Promotion from logical and expression
            right_->EmitRISC(stream, context, destReg);
        } else {
            std::string label2 {context.MakeLabel(".L_OR")};
            std::string label3 {context.MakeLabel(".L_OR")};
            std::string label4 {context.MakeLabel(".L_OR")};
            Register leftReg = context.AllocateTemporary();
            Utils::EmitComparison(stream, context, leftReg, *left_);
            stream << "bne " << leftReg << ",zero," << label2 << std::endl;
            context.FreeRegister(leftReg);
            Register rightReg = context.AllocateTemporary();
            Utils::EmitComparison(stream, context, rightReg, *right_);
            stream << "beq " << rightReg << ",zero," << label3 << std::endl;
            context.FreeRegister(rightReg);
            stream << label2 << ":" << std::endl;
            stream << "li " << destReg << ",1" << std::endl;
            stream << "j " << label4 << std::endl;
            stream << label3 << ":" << std::endl;
            stream << "li " << destReg << ",0" << std::endl;
            stream << label4 << ":" << std::endl;
        }
    }

    void LogicalOrExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr) {
            left_->Print(stream);
            stream << " || ";
        }
        right_->Print(stream);
    }


    TypeSpecifier LogicalOrExpression::GetType(Context& context) const {
        if (left_ == nullptr) {
            return right_->GetType(context);
        }

        return TypeSpecifier::INT;
    }

    bool LogicalOrExpression::ContainsFunctionCall() const {
        return (left_ != nullptr && left_->ContainsFunctionCall()) || right_->ContainsFunctionCall();
    }

    std::string LogicalOrExpression::GetGlobalIdentifier() const {
        return right_->GetGlobalIdentifier();
    }

    int LogicalOrExpression::Evaluate(Context &context) const {
        if (left_ == nullptr) {
            return right_->Evaluate(context);
        }

        return left_->Evaluate(context) || right_->Evaluate(context);
    }

    double LogicalOrExpression::EvaluateFloat(ast::Context &context) const {
        if (left_ == nullptr) {
            return right_->EvaluateFloat(context);
        }

        return static_cast<bool>(left_->EvaluateFloat(context)) || static_cast<bool>(right_->EvaluateFloat(context));
    }

}
