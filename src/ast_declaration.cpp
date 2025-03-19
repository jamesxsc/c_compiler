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
                bool useFloat = type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE;
                Register tempReg = context.AllocateTemporary(stream, useFloat);
                if (initDeclarator->IsArray()) {
                    assert(initDeclarator->GetInitializer().IsList() && "Array initializer must be a list");
                    Variable array = context.CurrentFrame().bindings.InsertOrOverwrite(identifier,
                                                                                       initDeclarator->BuildArray(type,
                                                                                                                  context));
                    // ^ Wraps the type, but we keep element type here
                    const auto &initializerList = static_cast<const InitializerList &>(initDeclarator->GetInitializer()); // NOLINT(*-pro-type-static-cast-downcast)
                    int idx = 0;
                    for (const auto &initializer: initializerList) {
                        initializer->EmitRISC(stream, context, tempReg);
                        switch (type) {
                            case TypeSpecifier::INT:
                            case TypeSpecifier::UNSIGNED:
                            case TypeSpecifier::POINTER:
                            case TypeSpecifier::ENUM:
                                stream << "sw " << tempReg << "," << array.offset + idx * type.GetTypeSize() << "(s0)"
                                       << std::endl;
                                break;
                            case TypeSpecifier::FLOAT:
                                stream << "fsw " << tempReg << "," << array.offset + idx * type.GetTypeSize() << "(s0)"
                                       << std::endl;
                                break;
                            case TypeSpecifier::DOUBLE:
                                stream << "fsd " << tempReg << "," << array.offset + idx * type.GetTypeSize() << "(s0)"
                                       << std::endl;
                                break;
                            case TypeSpecifier::CHAR:
                                stream << "sb " << tempReg << "," << array.offset + idx * type.GetTypeSize() << "(s0)"
                                       << std::endl;
                                break;
                            case TypeSpecifier::STRUCT: {
                                assert(initializer->IsList() && "Struct initializer must be a list");
                                const auto &structInitializerList = static_cast<const InitializerList &>(*initializer); // NOLINT(*-pro-type-static-cast-downcast)
                                auto it = structInitializerList.begin();
                                int memberOffset = 0;
                                for (const auto &member: type.GetStructMembers()) {
                                    if (member.first == "#padding") {
                                        stream << "li " << tempReg << ",0" << std::endl;
                                        stream << "sb " << tempReg << ","
                                               << array.offset + idx * type.GetTypeSize() + memberOffset << "(s0)"
                                               << std::endl;
                                        memberOffset += member.second.GetTypeSize();
                                        continue;
                                    }

                                    // Incomplete init list
                                    if (it == initializerList.end()) {
                                        stream << ".zero " << member.second.GetTypeSize() << std::endl;
                                        memberOffset += member.second.GetTypeSize();
                                        ++it;
                                        continue;
                                    }

                                    TypeSpecifier memberType = member.second;
                                    switch (memberType) {
                                        case TypeSpecifier::Type::INT:
                                        case TypeSpecifier::Type::UNSIGNED:
                                        case TypeSpecifier::Type::POINTER:
                                        case TypeSpecifier::Type::ENUM:
                                            (*it)->EmitRISC(stream, context, tempReg);
                                            stream << "sw " << tempReg << ","
                                                   << array.offset + idx * type.GetTypeSize() + memberOffset << "(s0)"
                                                   << std::endl;
                                            break;
                                        case TypeSpecifier::Type::CHAR:
                                            (*it)->EmitRISC(stream, context, tempReg);
                                            stream << "sb " << tempReg << ","
                                                   << array.offset + idx * type.GetTypeSize() + memberOffset << "(s0)"
                                                   << std::endl;
                                            break;
                                        case TypeSpecifier::Type::FLOAT:
                                        case TypeSpecifier::Type::DOUBLE: {
                                            Register tempFloatReg = context.AllocateTemporary(stream, true);
                                            (*it)->EmitRISC(stream, context, tempFloatReg);
                                            stream << (memberType == TypeSpecifier::Type::FLOAT ? "fsw " : "fsd ")
                                                   << tempFloatReg << ","
                                                   << array.offset + idx * type.GetTypeSize() + memberOffset << "(s0)"
                                                   << std::endl;
                                            context.FreeTemporary(tempFloatReg, stream);
                                            break;
                                        }
                                        case TypeSpecifier::Type::ARRAY:
                                        case TypeSpecifier::Type::STRUCT: // todo flatten nested structs - or maybe not flatten here
                                        case TypeSpecifier::Type::VOID:
                                            throw std::runtime_error(
                                                    "Declaration::EmitRISC() called on an unsupported struct member type");
                                    }
                                    ++it;
                                    memberOffset += memberType.GetTypeSize();
                                }
                                break;
                            }
                            case TypeSpecifier::ARRAY: // todo multidim
                            case TypeSpecifier::VOID:
                                throw std::runtime_error(
                                        "Declaration::EmitRISC() called on an unsupported array type");
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
                        case TypeSpecifier::ENUM:
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
                        case TypeSpecifier::STRUCT: {
                            assert(initDeclarator->GetInitializer().IsList() && "Struct initializer must be a list");
                            int memberOffset = 0;
                            const auto &initializerList = static_cast<const InitializerList &>(initDeclarator->GetInitializer()); // NOLINT(*-pro-type-static-cast-downcast)
                            auto it = initializerList.begin();
                            for (const auto &member: type.GetStructMembers()) {
                                if (member.first == "#padding") {
                                    stream << "li " << tempReg << ",0" << std::endl;
                                    stream << "sb " << tempReg << "," << var.offset + memberOffset << "(s0)"
                                           << std::endl;
                                    memberOffset += member.second.GetTypeSize();
                                    continue;
                                }

                                // Incomplete init list
                                if (it == initializerList.end()) {
                                    stream << ".zero " << member.second.GetTypeSize() << std::endl;
                                    memberOffset += member.second.GetTypeSize();
                                    ++it;
                                    continue;
                                }

                                TypeSpecifier memberType = member.second;
                                switch (memberType) {
                                    case TypeSpecifier::Type::INT:
                                    case TypeSpecifier::Type::UNSIGNED:
                                    case TypeSpecifier::Type::POINTER:
                                    case TypeSpecifier::Type::ENUM:
                                        (*it)->EmitRISC(stream, context, tempReg);
                                        stream << "sw " << tempReg << "," << var.offset + memberOffset << "(s0)"
                                               << std::endl;
                                        break;
                                    case TypeSpecifier::Type::CHAR:
                                        (*it)->EmitRISC(stream, context, tempReg);
                                        stream << "sb " << tempReg << "," << var.offset + memberOffset << "(s0)"
                                               << std::endl;
                                        break;
                                    case TypeSpecifier::Type::FLOAT:
                                    case TypeSpecifier::Type::DOUBLE: {
                                        Register tempFloatReg = context.AllocateTemporary(stream, true);
                                        (*it)->EmitRISC(stream, context, tempFloatReg);
                                        stream << (memberType == TypeSpecifier::Type::FLOAT ? "fsw " : "fsd ")
                                               << tempFloatReg << "," << var.offset + memberOffset << "(s0)"
                                               << std::endl;
                                        context.FreeTemporary(tempFloatReg, stream);
                                        break;
                                    }
                                    case TypeSpecifier::Type::ARRAY:
                                    case TypeSpecifier::Type::STRUCT:
                                    case TypeSpecifier::Type::VOID:
                                        throw std::runtime_error(
                                                "Declaration::EmitRISC() called on an unsupported struct member type");
                                }
                                it++;
                                memberOffset += memberType.GetTypeSize();
                            }
                            break;
                        }
                        case TypeSpecifier::VOID:
                        case TypeSpecifier::ARRAY: // Should never get here
                            throw std::runtime_error(
                                    "ArrayIndexExpression::EmitRISC() called on an unsupported type");
                    }
                }
                context.FreeTemporary(tempReg, stream);
            } else {
                // Allocated (stack), but not initialized
                if (initDeclarator->IsArray()) {
                    context.CurrentFrame().bindings.InsertOrOverwrite(identifier,
                                                                      initDeclarator->BuildArray(type, context));
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
