#include "ast_function_declarator.hpp"
#include "ast_identifier.hpp"
#include "ast_parameter_declaration.hpp"

namespace ast {

    void FunctionDeclarator::EmitRISC(std::ostream &stream, Context &context, int destReg) const {
        // TODO a better way to distinguish identifier risc for fn label and lw for variable (cx parser)
        // Yes, split into direct_declarator and primary_expression both take string as constructor
        stream << ".globl " << GetIdentifier() << std::endl;
        stream << GetIdentifier() << ":" << std::endl;
        // TODO ^^^^ assembly parameter list?
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
        if (parameterList_) {
            int idx = 0;
            for (const auto& param: *parameterList_) {
                // TODO types sizes etc 
                // TODO URGENT force types earlier eg in the parser and members/constructors to avoid these disgraceful casts
                int offset = -frameSize + 4 * (static_cast<int>(parameterList_->Size()) - idx);
                context.CurrentFrame().bindings.insert({param->GetIdentifier(), Variable{
                        .offset = offset,
                        .size = 4,
                        .reg = -1
                }});
                stream << "sw a" << idx << ", " << offset << "(s0)" << std::endl;
                ++idx;
            }
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

} // namespace ast
