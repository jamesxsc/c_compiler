#include "ast_function_declarator.hpp"
#include "ast_parameter_declaration.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void FunctionDeclarator::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        stream << ".globl " << GetIdentifier() << std::endl;
        stream << GetIdentifier();
        stream << ":" << std::endl;
        int frameSize = 512; // bytes // TODO dynamic size - involves decision whether to fix size before generating prologue
        context.PushFrame({
                                  .size = frameSize,
                                  .bindings = {}
                          });

        // Stack/frame pointer/return address setup
        stream << "addi sp, sp, -" << frameSize << std::endl;
        stream << "sw ra, " << frameSize - 4 << "(sp)" << std::endl;
        stream << "sw s0, " << frameSize - 8 << "(sp)" << std::endl;
        stream << "addi s0, sp, " << frameSize << std::endl;

        // Temp persist temporaries (testing)
        stream << "sw t0, " << frameSize - 12 << "(sp)" << std::endl;
        stream << "sw t1, " << frameSize - 16 << "(sp)" << std::endl;
        stream << "sw t2, " << frameSize - 20 << "(sp)" << std::endl;
        stream << "sw t3, " << frameSize - 24 << "(sp)" << std::endl;
        stream << "sw t4, " << frameSize - 28 << "(sp)" << std::endl;
        stream << "sw t5, " << frameSize - 32 << "(sp)" << std::endl;
        stream << "sw t6, " << frameSize - 36 << "(sp)" << std::endl;

        // Store args
        if (parameterList_) {
            parameterList_->EmitRISC(stream, context, destReg);
        }
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