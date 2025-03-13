#include <cassert>
#include "ast_declaration.hpp"
#include "ast_initializer_list.hpp"

namespace ast {

    // Non-global declarations
    void Declaration::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (IsTypedef()) return;

        for (const auto &initDeclarator: *initDeclaratorList_) {
            // Handle local variable declarations
            TypeSpecifier type = GetType(context);
            int size = GetTypeSize(type);
            std::string identifier = initDeclarator->GetIdentifier();
            bool useFloat = type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE;
            // We won't ever need to "return" to destReg to overwrite it for temporary use
            destReg = context.AllocateTemporary(useFloat);
            // Bindings and init
            // Don't handle types with multiple keywords (for now)
            assert(!declarationSpecifiers_->GetTypeSpecifiers().empty() && "Declaration must have a type specifier");
            if (initDeclarator->HasInitializer()) {
                if (initDeclarator->IsArray()) {
                    // This approach may have to change slightly for (nested) structs
                    assert(initDeclarator->GetInitializer().IsList() && "Array initializer must be a list");
                    Variable array = context.CurrentFrame().bindings.InsertOrOverwrite(identifier, initDeclarator->BuildArray(type, context));
                    const auto &initializerList = static_cast<const InitializerList &>(initDeclarator->GetInitializer()); // NOLINT(*-pro-type-static-cast-downcast)
                    int idx = 0;
                    for (const auto & initializer : initializerList) {
                        initializer->EmitRISC(stream, context, destReg);
                        switch (type) {
                            case TypeSpecifier::INT:
                                stream << "sw " << destReg << "," << array.offset + idx * GetTypeSize(type) << "(s0)" << std::endl;
                                break;
                            case TypeSpecifier::FLOAT:
                                stream << "fsw " << destReg << "," << array.offset + idx * GetTypeSize(type) << "(s0)" << std::endl;
                                break;
                            case TypeSpecifier::DOUBLE:
                                stream << "fsd " << destReg << "," << array.offset + idx * GetTypeSize(type) << "(s0)" << std::endl;
                                break;
                            case TypeSpecifier::CHAR:
                                stream << "sb " << destReg << "," << array.offset + idx * GetTypeSize(type) << "(s0)" << std::endl;
                                break;
                            case TypeSpecifier::POINTER:
                                throw std::runtime_error("Array of pointers not supported");
                        }
                        idx++;
                    }
                } else {
                    // Generates initializer/assignment code
                    initDeclarator->EmitRISC(stream, context, destReg);

                    Variable var = context.CurrentFrame().bindings.InsertOrOverwrite(identifier, Variable{
                            .size = initDeclarator->IsPointer() ? 4 : size,
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
                            stream << "sb " << destReg << "," << var.offset << "(s0)" << std::endl;
                            break;
                    }
                }
            } else {
                // Allocated (stack), but not initialized
                if (initDeclarator->IsArray()) {
                    context.CurrentFrame().bindings.InsertOrOverwrite(identifier, initDeclarator->BuildArray(type, context));
                } else {
                    context.CurrentFrame().bindings.InsertOrOverwrite(identifier, Variable{
                            .size = initDeclarator->IsPointer() ? 4 : size,
                            .reg = Register::zero,
                            .type = initDeclarator->IsPointer() ? TypeSpecifier::POINTER : type
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

    bool Declaration::IsTypedef() const {
        return declarationSpecifiers_->GetStorageClassSpecifier() == StorageClassSpecifier::Typedef;
    }

}
