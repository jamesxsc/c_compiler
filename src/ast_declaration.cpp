#include <cassert>
#include "ast_declaration.hpp"

namespace ast {

    void Declaration::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        for (const auto &initDeclarator: *initDeclaratorList_) {
            // Handle local variable declarations
            TypeSpecifier type = GetType(context);
            int size = GetTypeSize(type);
            std::string identifier = initDeclarator->GetIdentifier();
            bool useFloat = type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE;
            destReg = context.AllocateTemporary(useFloat);

            // todo check if using destreg is safe here (don't use zero as a temporary)
            // Bindings and init
            // Don't handle types with multiple keywords (for now)
            assert(!declarationSpecifiers_->GetTypeSpecifiers().empty() && "Declaration must have a type specifier");
            if (initDeclarator->HasInitializer()) {
                // todo array case here
                // Generates initializer/assignment code
                initDeclarator->EmitRISC(stream, context, destReg);

                Variable var = context.CurrentFrame().bindings.InsertOrOverwrite(identifier, Variable{
                        .size = size,
                        .reg = destReg,
                        .type = initDeclarator->IsPointer() ? TypeSpecifier::POINTER : type
                });
                switch (type) {
                    // todo would you like some typechecking asserts sir?
                    case TypeSpecifier::INT:
                        stream << "sw " << destReg << "," << var.offset << "(s0)" << std::endl;
                        break;
                    case TypeSpecifier::POINTER:
                        stream << "sw " << destReg << "," << var.offset << "(s0)" << std::endl;
                        break;
                    case TypeSpecifier::FLOAT:
                        stream << "fsw " << destReg << "," << var.offset << "(s0)" << std::endl;
                        break;
                    case TypeSpecifier::DOUBLE:
                        stream << "fsd " << destReg << "," << var.offset << "(s0)" << std::endl;
                        break;
                    case TypeSpecifier::CHAR:
                        // todo check char store in godbolt
                        stream << "sb " << destReg << "," << var.offset << "(s0)" << std::endl;
                        break;
                }
            } else {
                // Allocated, but not initialized
                if (initDeclarator->IsPointer()) {
                    context.CurrentFrame().bindings.InsertOrOverwrite(identifier, Variable{
                            .size = size,
                            .reg = Register::zero,
                            .type = TypeSpecifier::POINTER
                    });
                } // todo array case here
                else {
                    context.CurrentFrame().bindings.InsertOrOverwrite(identifier, Variable{
                            .size = size,
                            .reg = Register::zero,
                            .type = type
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
