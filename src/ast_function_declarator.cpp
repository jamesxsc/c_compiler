#include "ast_function_declarator.hpp"
#include "ast_identifier.hpp"
#include "ast_parameter_declaration.hpp"

namespace ast {

    void FunctionDeclarator::EmitRISC(std::ostream &stream, Context &context, int destReg) const {
        stream << ".globl " << GetIdentifier() << std::endl;
        stream << GetIdentifier();
        parameterList_->EmitLabelRISC(stream);
        stream << ":" << std::endl;
        int frameSize = 32; // bytes // TODO dynamic size
        context.PushFrame({
                                  .size = frameSize,
                                  .bindings = {}
                          });

        // Stack/frame pointer/return address setup
        stream << "addi sp, sp, -" << frameSize << std::endl;
        stream << "sw ra, " << frameSize - 4 << "(sp)" << std::endl;
        stream << "sw s0, " << frameSize - 8 << "(sp)" << std::endl;
        stream << "addi s0, sp, " << frameSize << std::endl;

        // Store args
        parameterList_->EmitRISC(stream, context, destReg);
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

} // namespace ast
