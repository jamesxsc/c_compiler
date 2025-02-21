#include "ast_function_declarator.hpp"
#include "ast_identifier.hpp"
#include "ast_declaration.hpp"

namespace ast {

    void FunctionDeclarator::EmitRISC(std::ostream &stream, Context &context, int destReg) const {
        // TODO a better way to distinguish identifier risc for fn label and lw for variable
        auto identifier = dynamic_cast<const Identifier *>(identifier_.get());
        stream << identifier->GetIdentifier() << ":" << std::endl;
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
        if (parameters_) {
            int idx = 0;
            for (const auto &it: *parameters_) {
                // TODO types sizes etc 
                auto param = dynamic_cast<const Declaration *>(it.get());
                // Consider forcing types earlier eg in the parser to avoid these disgraceful casts
                auto paramIdentifier = dynamic_cast<const Identifier *>(param->GetIdentifier().get());
                int offset = -frameSize + 4 * (static_cast<int>(parameters_->Size()) - idx);
                context.CurrentFrame().bindings.insert({paramIdentifier->GetIdentifier(), Variable{
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
        identifier_->Print(stream);
        stream << "(";
        // Print with comma separation
        if (parameters_) {
            for (auto it = parameters_->begin(); it != parameters_->end(); ++it) {
                (*it)->Print(stream);
                if (it + 1 != parameters_->end()) {
                    stream << ", ";
                }
            }
        }
        stream << ")";
    }

} // namespace ast
