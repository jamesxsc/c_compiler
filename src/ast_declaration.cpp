#include <cassert>
#include "ast_declaration.hpp"
#include "ast_initializer_list.hpp"

namespace ast {

    // Non-global declarations
    void Declaration::EmitRISC(std::ostream &stream, Context &context, Register) const {
        if (IsTypedef()) return;

        for (const auto &initDeclarator: *initDeclaratorList_) {
            // Handle local variable declarations
            TypeSpecifier type = declarationSpecifiers_->GetType(context);
            if (initDeclarator->IsPointer())
                type = TypeSpecifier(TypeSpecifier::POINTER, type);

            std::string identifier = initDeclarator->GetIdentifier();
            // We won't ever need to "return" to destReg so overwrite it for temporary use
            // Bindings and init
            if (initDeclarator->HasInitializer()) {
                bool useFloat = type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE; // This should work, type is element type in array case here
                Register tempReg = context.AllocateTemporary(useFloat);
                if (initDeclarator->IsArray()) {
                    // This approach may have to change slightly for (nested) structs
                    assert(initDeclarator->GetInitializer().IsList() && "Array initializer must be a list");
                    Variable array = context.CurrentFrame().bindings.InsertOrOverwrite(identifier, initDeclarator->BuildArray(type, context));
                    // ^ Wraps the type, but we keep element type here
                    const auto &initializerList = static_cast<const InitializerList &>(initDeclarator->GetInitializer()); // NOLINT(*-pro-type-static-cast-downcast)
                    int idx = 0;
                    for (const auto & initializer : initializerList) {
                        initializer->EmitRISC(stream, context, tempReg);
                        switch (type) {
                            case TypeSpecifier::INT:
                            case TypeSpecifier::UNSIGNED:
                            case TypeSpecifier::POINTER:
                            case TypeSpecifier::ENUM:
                                stream << "sw " << tempReg << "," << array.offset + idx * type.GetTypeSize() << "(s0)" << std::endl;
                                break;
                            case TypeSpecifier::FLOAT:
                                stream << "fsw " << tempReg << "," << array.offset + idx * type.GetTypeSize() << "(s0)" << std::endl;
                                break;
                            case TypeSpecifier::DOUBLE:
                                stream << "fsd " << tempReg << "," << array.offset + idx * type.GetTypeSize() << "(s0)" << std::endl;
                                break;
                            case TypeSpecifier::CHAR:
                                stream << "sb " << tempReg << "," << array.offset + idx * type.GetTypeSize() << "(s0)" << std::endl;
                                break;
                            case TypeSpecifier::VOID:
                            case TypeSpecifier::STRUCT:
                            case TypeSpecifier::ARRAY:
                                throw std::runtime_error(
                                        "Declaration::EmitRISC() called on an unsupported array type");
                                // todo handle these
                        }
                        idx++;
                    }
                } else {
                    int size = type.GetTypeSize();
                    // Generates initializer/assignment code
                    initDeclarator->EmitRISC(stream, context, tempReg);

                    Variable var = context.CurrentFrame().bindings.InsertOrOverwrite(identifier, Variable{
                            .size = initDeclarator->IsPointer() ? 4 : size,
                            .type = type
                    });
                    switch (type) {
                        case TypeSpecifier::INT:
                        case TypeSpecifier::POINTER:
                        case TypeSpecifier::UNSIGNED:
                            stream << "sw " << tempReg << "," << var.offset << "(s0)" << std::endl;
                            break;
                        case TypeSpecifier::FLOAT:
                            stream << "fsw " << tempReg << "," << var.offset << "(s0)" << std::endl;
                            break;
                        case TypeSpecifier::DOUBLE:
                            stream << "fsd " << tempReg << "," << var.offset << "(s0)" << std::endl;
                            break;
                        case TypeSpecifier::CHAR:
                            stream << "sb " << tempReg << "," << var.offset << "(s0)" << std::endl;
                            break;
                        case TypeSpecifier::VOID:
                        case TypeSpecifier::ENUM:
                        case TypeSpecifier::STRUCT:
                        case TypeSpecifier::ARRAY:
                            throw std::runtime_error(
                                    "ArrayIndexExpression::EmitRISC() called on an unsupported type");
                            // todo handle these
                    }
                }
                context.FreeTemporary(tempReg);
            } else {
                // Allocated (stack), but not initialized
                if (initDeclarator->IsArray()) {
                    context.CurrentFrame().bindings.InsertOrOverwrite(identifier, initDeclarator->BuildArray(type, context));
                } else {
                    int size = type.GetTypeSize();
                    context.CurrentFrame().bindings.InsertOrOverwrite(identifier, Variable{
                            .size = initDeclarator->IsPointer() ? 4 : size,
                            .type = type
                    });
                }
            }
        }
    }

    void Declaration::Print(std::ostream &stream) const {
        declarationSpecifiers_->Print(stream);
        initDeclaratorList_->Print(stream);
    }

    bool Declaration::IsTypedef() const {
        return declarationSpecifiers_->GetStorageClassSpecifier() == StorageClassSpecifier::Typedef;
    }

    bool Declaration::IsAggregateTypeDefinition() const {
        return false;
    }

}
