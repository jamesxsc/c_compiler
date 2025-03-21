#include <sstream>
#include <cassert>
#include "ast_function_definition.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void FunctionDefinition::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Emit assembler directives.
        stream << ".text" << std::endl;
        stream << ".align 2" << std::endl;
        // .globl directive is handled by declarator
        declarator_->EmitLabelRISC(stream);

        // Store function return type and parameter sizes in context
        context.InsertFunction(declarator_->GetIdentifier(), declarator_->BuildFunction(GetType(context), context));

        // Push a new frame onto the stack
        // todo maybe we can do something pretty good here
        int frameSize = 1024; // bytes fixed until we get time to perform analysis of how large the frame needs to be
        context.PushFrame({
                                  .size = frameSize,
                                  .bindings = Bindings(frameSize,
                                                       -4 * 13), // 4 * 13 is the size of the saved registers (max case)
                          });
        context.CurrentFrame().returnType = GetType(context);
        // Set the return label for return statements to emit
        context.CurrentFrame().returnLabel = context.MakeLabel(".L_RETURN");

        // Execute the declarator and function body without emitting it to determine which registers need to be saved
        std::stringstream bodyStream;
        if (GetType(context).UseStack()) declarator_->SetHiddenPointerReturn();
        if (compound_statement_) {
            declarator_->EmitRISC(bodyStream, context, destReg); // Parameters
            compound_statement_->EmitRISC(bodyStream, context, destReg);
        }

        // Stack/frame pointer/return address setup
        stream << "addi sp,sp,-" << frameSize << std::endl;
        stream << "sw ra," << frameSize - 4 << "(sp)" << std::endl;

        // Save used persistent registers s0-s11
        int storedCount = 0;
        for (int r = 0; r < 12; r++) {
            if (context.CurrentFrame().usedIntegerPersistentRegisters.test(r)) {
                stream << "sw s" << r << "," << frameSize - 8 - storedCount * 4 << "(sp)" << std::endl;
                storedCount++;
            }
        }
        if (storedCount % 2 == 0) {
            storedCount++; // Ensure double alignment
        }
        for (int r = 0; r < 8; r++) {
            if (context.CurrentFrame().usedFloatPersistentRegisters.test(r)) {
                stream << "fsd fs" << r << "," << frameSize - 8 - storedCount * 4 << "(sp)" << std::endl;
                storedCount += 2; // Float registers are 64 bit
            }
        }

        // Set the frame pointer
        stream << "addi s0,sp," << frameSize << std::endl;

        // Now emit the parameters and function body
        if (bodyStream.rdbuf()->in_avail() > 0)
            stream << bodyStream.rdbuf();

        // Add the return RISC (that all return statements jump to)
        stream << *context.CurrentFrame().returnLabel << ":" << std::endl;

        // Stack/frame pointer/return address teardown
        stream << "lw ra," << frameSize - 4 << "(sp)" << std::endl;

        // Restore used persistent registers
        storedCount = 0;
        for (int r = 0; r < 12; r++) {
            if (context.CurrentFrame().usedIntegerPersistentRegisters.test(r)) {
                stream << "lw s" << r << "," << frameSize - 8 - storedCount * 4 << "(sp)" << std::endl;
                storedCount++;
            }
        }
        if (storedCount % 2 == 0) {
            storedCount++; // Ensure double alignment
        }
        for (int r = 0; r < 8; r++) {
            if (context.CurrentFrame().usedFloatPersistentRegisters.test(r)) {
                stream << "fld fs" << r << "," << frameSize - 8 - storedCount * 4 << "(sp)" << std::endl;
                storedCount += 2; // Float registers are 64 bit
            }
        }

        stream << "addi sp,sp," << context.CurrentFrame().size << std::endl;
        stream << "ret" << std::endl;

        // Moved from return to fix multiple return statements/return in scope
        context.PopFrame();
    }

    void FunctionDefinition::Print(std::ostream &stream) const {
        declaration_specifiers_->Print(stream);

        if (declarator_->GetPointerReturn())
            stream << "*";

        declarator_->Print(stream);

        if (compound_statement_ != nullptr) {
            compound_statement_->Print(stream);
        }
    }


    TypeSpecifier FunctionDefinition::GetType(Context &context) const {
        TypeSpecifier returnType = declaration_specifiers_->GetType(context);
        if (declarator_->GetPointerReturn())
            returnType = {TypeSpecifier::POINTER, returnType};
        return returnType;
    }

}