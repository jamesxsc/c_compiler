#include "ast_declaration.hpp"

namespace ast {

    void Declaration::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        for (const auto &initDeclarator: *initDeclaratorList_) {
            // Stack work is done here
            // TODO need to handle different sizes (inferred from type)
            int size = 4;
            std::string identifier = initDeclarator->GetIdentifier();

            destReg = context.AllocateTemporary();

            // Increment frame down
            context.CurrentFrame().size += size;
            stream << "addi sp, sp, -" << size << std::endl;

            // Bindings and init
            // Note that 17's insert or assign is used to overwrite variables with the same name if they are refined in a block scope
            if (initDeclarator->HasInitializer()) {
                switch (typeSpecifier_) {
                    case TypeSpecifier::INT:
                        // Generates initializer/assignment code
                        initDeclarator->EmitRISC(stream, context, destReg);

                        context.CurrentFrame().bindings.insert_or_assign(identifier, Variable{
                                .offset = context.CurrentFrame().size,
                                .size = size,
                                .reg = destReg
                        });

                        stream << "sw " << destReg << ",0(sp)" << std::endl;
                        break;
                }
            } else {
                // Allocated, but not initialized
                context.CurrentFrame().bindings.insert_or_assign(identifier, Variable{
                        .offset = context.CurrentFrame().size,
                        .size = size,
                        .reg = Register::zero
                });
            }
        }
    }

    void Declaration::Print(std::ostream &stream) const {
        stream << typeSpecifier_; // This is overloaded to provide a string representation
        initDeclaratorList_->Print(stream);
    }

}
