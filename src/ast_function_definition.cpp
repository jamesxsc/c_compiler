#include "ast_function_definition.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void FunctionDefinition::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Emit assembler directives.
        // TODO investigate directives and what is actually required for the scope of this project
        stream << ".text" << std::endl;
        stream << ".align 2" << std::endl;
        // .globl directive is handled by declarator

        declarator_->EmitRISC(stream, context, destReg);

        if (compound_statement_ != nullptr) {
            compound_statement_->EmitRISC(stream, context, destReg);
        }
    }

    void FunctionDefinition::Print(std::ostream &stream) const {
        stream << declaration_specifiers_ << " ";

        declarator_->Print(stream);

        stream << "{" << std::endl;
        if (compound_statement_ != nullptr) {
            compound_statement_->Print(stream);
        }
        stream << "}" << std::endl;
    }


    ast::Type FunctionDefinition::GetType(Context&) const {
        return ast::Type(ast::TypeSpecifier::INT, true);
    }

}