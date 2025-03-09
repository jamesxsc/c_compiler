#include <cassert>
#include "ast_declaration.hpp"

namespace ast {

    void Declaration::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        for (const auto &initDeclarator: *initDeclaratorList_) {
            // Handle local variable declarations
            // TODO need to handle different sizes (inferred from type)
            int size = 4;
            std::string identifier = initDeclarator->GetIdentifier();
            destReg = context.AllocateTemporary();

            // Bindings and init
            // Don't handle types with multiple keywords (for now)
            assert(!declarationSpecifiers_->GetTypeSpecifiers().empty() && "Declaration must have a type specifier");
            TypeSpecifier typeSpecifier = declarationSpecifiers_->GetTypeSpecifiers().front();
            if (initDeclarator->HasInitializer()) {
                if (initDeclarator->IsPointer()) {
                    // Probably a unary & address of (this obtains an address)
                    initDeclarator->EmitRISC(stream, context, destReg);

                    Variable var = context.CurrentFrame().bindings.InsertOrOverwrite(identifier, Variable{
                            .size = size,
                            .reg = destReg,
                            .type = TypeSpecifier::POINTER
                    });

                    stream << "sw " << destReg << "," << var.offset << "(s0)" << std::endl;
                } else {
                    switch (typeSpecifier) {
                        case TypeSpecifier::INT: {
                            // Generates initializer/assignment code
                            initDeclarator->EmitRISC(stream, context, destReg);

                            Variable var = context.CurrentFrame().bindings.InsertOrOverwrite(identifier, Variable{
                                    .size = size,
                                    .reg = destReg,
                                    .type = typeSpecifier
                            });

                            stream << "sw " << destReg << "," << var.offset << "(s0)" << std::endl;
                            break;
                        }
                        case TypeSpecifier::POINTER:
                            // probably will never happen? this is the pointed to type
                            break;
                    }
                }
            } else {
                // Allocated, but not initialized
                if (initDeclarator->IsPointer()) {
                    context.CurrentFrame().bindings.InsertOrOverwrite(identifier, Variable{
                            .size = size,
                            .reg = Register::zero,
                            .type = TypeSpecifier::POINTER
                    });
                } else {
                    context.CurrentFrame().bindings.InsertOrOverwrite(identifier, Variable{
                            .size = size,
                            .reg = Register::zero,
                            .type = typeSpecifier
                    });
                }
            }
            context.FreeTemporary(destReg);
        }
    }

    void Declaration::Print(std::ostream &stream) const {
        declarationSpecifiers_->Print(stream);
        initDeclaratorList_->Print(stream);
    }

    TypeSpecifier Declaration::GetType(Context &context) const {
        assert(!declarationSpecifiers_->GetTypeSpecifiers().empty() && "Declaration must have a type specifier");
        return declarationSpecifiers_->GetTypeSpecifiers().front();
    }

}
