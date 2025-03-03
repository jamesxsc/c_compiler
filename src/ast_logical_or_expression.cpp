#include "ast_logical_or_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void LogicalOrExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (left_ == nullptr) { // Promotion from logical and expression
            right_->EmitRISC(stream, context, destReg);
        } else {
            // TODO operation depends on types - add a switch on child type(s?)
            Register leftReg = context.AllocateTemporary();
            std::string label2 {context.MakeLabel(".L_OR")};
            std::string label3 {context.MakeLabel(".L_OR")};
            std::string label4 {context.MakeLabel(".L_OR")};
            left_->EmitRISC(stream, context, leftReg);
            stream << "bne " << leftReg << ",zero," << label2 << std::endl;
            context.FreeTemporary(leftReg);
            Register rightReg = context.AllocateTemporary();
            right_->EmitRISC(stream, context, rightReg);
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


    ast::Type LogicalOrExpression::GetType(Context&) const {
        return ast::Type(ast::TypeSpecifier::INT, true);
    }

    bool LogicalOrExpression::ContainsFunctionCall() const {
        return (left_ != nullptr && left_->ContainsFunctionCall()) || right_->ContainsFunctionCall();
    }

}
