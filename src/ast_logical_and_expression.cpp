#include "ast_logical_and_expression.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void LogicalAndExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (left_ == nullptr) { // Promotion from inclusive or expression
            right_->EmitRISC(stream, context, destReg);
        } else {
            // TODO operation depends on types - add a switch on child type(s?)
            // This is for int
            Register leftReg = context.AllocateTemporary();
            std::string label2{context.MakeLabel(".L_AND")};
            std::string label3{context.MakeLabel(".L_AND")};
            left_->EmitRISC(stream, context, leftReg);
            stream << "beq " << leftReg << ",zero," << label2 << std::endl;
            context.FreeTemporary(leftReg);
            Register rightReg = context.AllocateTemporary();
            right_->EmitRISC(stream, context, rightReg);
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


    ast::Type LogicalAndExpression::GetType(Context&) const {
        return ast::Type(ast::TypeSpecifier::INT, true);
    }
}

