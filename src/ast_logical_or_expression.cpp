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
            context.FreeTemporary(leftReg);
            Register rightReg = context.AllocateTemporary();
            Utils::EmitComparison(stream, context, rightReg, *right_);
            stream << "beq " << rightReg << ",zero," << label3 << std::endl;
            context.FreeTemporary(rightReg);
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
        return right_->GetType(context);
    }

    bool LogicalOrExpression::ContainsFunctionCall() const {
        return (left_ != nullptr && left_->ContainsFunctionCall()) || right_->ContainsFunctionCall();
    }

    std::string LogicalOrExpression::GetGlobalIdentifier() const {
        return right_->GetGlobalIdentifier();
    }

    int LogicalOrExpression::GetGlobalValue() const {
        return right_->GetGlobalValue();
    }

}
