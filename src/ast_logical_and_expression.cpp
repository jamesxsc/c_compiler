#include "ast_logical_and_expression.hpp"
#include "ast_type_specifier.hpp"
#include "risc_utils.hpp"

namespace ast {

    void LogicalAndExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (left_ == nullptr) { // Promotion from inclusive or expression
            right_->EmitRISC(stream, context, destReg);
        } else {
            std::string label2{context.MakeLabel(".L_AND")};
            std::string label3{context.MakeLabel(".L_AND")};
            Register leftReg = context.AllocateTemporary();
            Utils::EmitComparison(stream, context, leftReg, *left_);
            stream << "beq " << leftReg << ",zero," << label2 << std::endl;
            context.FreeTemporary(leftReg);
            Register rightReg = context.AllocateTemporary();
            Utils::EmitComparison(stream, context, leftReg, *right_);
            stream << "beq " << rightReg << ",zero," << label2 << std::endl;
            context.FreeTemporary(rightReg);
            stream << "li " << destReg << ",1" << std::endl;
            stream << "j " << label3 << std::endl;
            stream << label2 << ":" << std::endl;
            stream << "li " << destReg << ",0" << std::endl;
            stream << label3 << ":" << std::endl;
        }
    }

    void LogicalAndExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr) {
            left_->Print(stream);
            stream << " && ";
        }
        right_->Print(stream);
    }


    TypeSpecifier LogicalAndExpression::GetType(Context& context) const {
        if (left_ == nullptr) {
            return right_->GetType(context);
        }

        return TypeSpecifier::INT;
    }

    bool LogicalAndExpression::ContainsFunctionCall() const {
        return (left_ != nullptr && left_->ContainsFunctionCall()) || right_->ContainsFunctionCall();
    }

    std::string LogicalAndExpression::GetGlobalIdentifier() const {
        return right_->GetGlobalIdentifier();
    }

    int LogicalAndExpression::Evaluate(Context &context) const {
        if (left_ == nullptr) {
            return right_->Evaluate(context);
        }

        return left_->Evaluate(context) && right_->Evaluate(context);
    }

}

