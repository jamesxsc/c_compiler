#include "ast_declaration.hpp"

namespace ast {

    void Declaration::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        for (const auto &initDeclarator: *initDeclaratorList_) {
            // Handle forward declarations
            if (initDeclarator->IsFunction()) {
                // TODO store functions in context because we will need size etc information


            } else { // Handle variable declarations
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
                    if (initDeclarator->IsPointer()) {
                        // Probably a unary & address of (this obtains an address)
                        initDeclarator->EmitRISC(stream, context, destReg);

                        context.CurrentFrame().bindings.insert_or_assign(identifier, Variable{
                                .offset = context.CurrentFrame().size,
                                .size = size,
                                .reg = destReg,
                                .type = TypeSpecifier::POINTER
                        });

                        stream << "sw " << destReg << "," << context.CurrentFrame().size << "(s0)" << std::endl;
                    } else {
                        switch (typeSpecifier_) {
                            case TypeSpecifier::INT:
                                // Generates initializer/assignment code
                                initDeclarator->EmitRISC(stream, context, destReg);

                                context.CurrentFrame().bindings.insert_or_assign(identifier, Variable{
                                        .offset = context.CurrentFrame().size,
                                        .size = size,
                                        .reg = destReg,
                                        .type = typeSpecifier_
                                });

                                stream << "sw " << destReg << "," << context.CurrentFrame().size << "(s0)" << std::endl;
                                break;
                            case TypeSpecifier::POINTER:
                                // probably will never happen? this is the pointed to type
                                break;
                        }
                    }
                } else {
                    if (initDeclarator->IsPointer()) {
                        context.CurrentFrame().bindings.insert_or_assign(identifier, Variable{
                                .offset = context.CurrentFrame().size,
                                .size = size,
                                .reg = Register::zero,
                                .type = TypeSpecifier::POINTER
                        });
                    } else {
                        // Allocated, but not initialized
                        context.CurrentFrame().bindings.insert_or_assign(identifier, Variable{
                                .offset = context.CurrentFrame().size,
                                .size = size,
                                .reg = Register::zero,
                                .type = typeSpecifier_
                        });
                    }
                }
            }
        }
    }

    void Declaration::Print(std::ostream &stream) const {
        stream << typeSpecifier_; // This is overloaded to provide a string representation
        initDeclaratorList_->Print(stream);
    }

}
