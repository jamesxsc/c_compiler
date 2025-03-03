#include "ast_function_declarator.hpp"
#include "ast_parameter_declaration.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void FunctionDeclarator::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Store args
        if (parameterList_) {
            parameterList_->EmitRISC(stream, context, destReg);
        }
    }

    void FunctionDeclarator::EmitLabelRISC(std::ostream &stream) const {
        stream << ".globl " << GetIdentifier() << std::endl;
        stream << GetIdentifier();
        stream << ":" << std::endl;
    }

    void FunctionDeclarator::Print(std::ostream &stream) const {
        stream << GetIdentifier();
        stream << "(";
        // Print with comma separation
        if (parameterList_) {
            for (auto it = parameterList_->begin(); it != parameterList_->end(); ++it) {
                (*it)->Print(stream);
                if (it + 1 != parameterList_->end()) {
                    stream << ", ";
                }
            }
        }
        stream << ")";
    }



    ast::Type FunctionDeclarator::GetType(Context&) const {
        return ast::Type(ast::TypeSpecifier::INT, true);
    }

}