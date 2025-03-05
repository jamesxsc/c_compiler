#include <sstream>
#include "ast_function_definition.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void FunctionDefinition::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Emit assembler directives.
        // TODO investigate directives and what is actually required for the scope of this project
        stream << ".text" << std::endl;
        stream << ".align 2" << std::endl;
        // .globl directive is handled by declarator
        declarator_->EmitLabelRISC(stream);

        // Push a new frame onto the stack
        int frameSize = 512; // bytes // fixed until we get time to perform analysis of how large the frame needs to be
        context.PushFrame({
                                  .size = frameSize,
                                  .bindings = Bindings(frameSize, -4 * 13), // 4 * 13 is the size of the saved registers (max case)
                          });
        // Execute the declarator and function body without emitting it to determine which registers need to be saved
        std::stringstream bodyStream;
        if (compound_statement_) {
            declarator_->EmitRISC(bodyStream, context, destReg); // Parameters
            compound_statement_->EmitRISC(bodyStream, context, destReg);
        }

        // Stack/frame pointer/return address setup
        stream << "addi sp, sp, -" << frameSize << std::endl;
        stream << "sw ra, " << frameSize - 4 << "(sp)" << std::endl;


        // Save used persistent registers s0-s11
        // TODO make this not waste space for unused ones
        for (int i = 0; i < 12; i++) {
            if (context.CurrentFrame().usedPersistentRegisters.test(i)) {
                stream << "sw s" << i << ", " << frameSize - 8 - i * 4 << "(sp)" << std::endl;
            }
        }

        // Set the frame pointer
        stream << "addi s0, sp, " << frameSize << std::endl;

        // Now emit the parameters and function body
        stream << bodyStream.rdbuf();

        // Moved from return to fix multiple return statements/return in scope
        context.PopFrame();
    }

    void FunctionDefinition::Print(std::ostream &stream) const {
        stream << declaration_specifiers_ << " ";

        declarator_->Print(stream);

        if (compound_statement_ != nullptr) {
            compound_statement_->Print(stream);
        }
    }


    TypeSpecifier FunctionDefinition::GetType(Context &) const {
        return declaration_specifiers_; // TODO cx and probably remove this member func from declarator
    }

}