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
            if (initDeclarator->IsArray())
                type = initDeclarator->BuildArray(type, context).type;

            std::string identifier = initDeclarator->GetIdentifier();

            // Bindings and init
            if (initDeclarator->HasInitializer()) {
                bool useFloat = type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE; // Only for non-arrays
                Register tempReg = context.AllocateTemporary(stream, useFloat);
                if (type.IsArray()) {
                    assert(initDeclarator->GetInitializer().IsList() && "Array initializer must be a list");
                    Variable array = context.CurrentFrame().bindings.InsertOrOverwrite(identifier, Variable{
                        .size = type.GetTypeSize(),
                        .type = type
                    });
                    // todo and tests for params with arrays/structs?

                    const auto &initializerList = static_cast<const InitializerList &>(initDeclarator->GetInitializer()); // NOLINT(*-pro-type-static-cast-downcast)
                    EmitArrayInitializer(stream, context, tempReg, initializerList, type.GetArrayType(), array.offset);
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
                            const auto &initializerList = static_cast<const InitializerList &>(initDeclarator->GetInitializer()); // NOLINT(*-pro-type-static-cast-downcast)
                            EmitStructInitializer(initializerList, type, tempReg, var.offset, stream, context);
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
                    context.CurrentFrame().bindings.InsertOrOverwrite(identifier, Variable{
                        .size = type.GetTypeSize(),
                        .type = type
                    });
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

    void Declaration::EmitStructInitializer(const InitializerList &initializerList, const TypeSpecifier &type,
                                            const Register &tempReg, int baseOffset, std::ostream &stream,
                                            Context &context) {
        int memberOffset = 0;
        auto it = initializerList.begin();
        for (const auto &member: type.GetStructMembers()) {
            if (member.first == "#padding") {
                stream << "li " << tempReg << ",0" << std::endl;
                stream << "sb " << tempReg << "," << baseOffset + memberOffset << "(s0)"
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
                    stream << "sw " << tempReg << "," << baseOffset + memberOffset << "(s0)"
                           << std::endl;
                    break;
                case TypeSpecifier::Type::CHAR:
                    (*it)->EmitRISC(stream, context, tempReg);
                    stream << "sb " << tempReg << "," << baseOffset + memberOffset << "(s0)"
                           << std::endl;
                    break;
                case TypeSpecifier::Type::FLOAT:
                case TypeSpecifier::Type::DOUBLE: {
                    Register tempFloatReg = context.AllocateTemporary(stream, true);
                    (*it)->EmitRISC(stream, context, tempFloatReg);
                    stream << (memberType == TypeSpecifier::Type::FLOAT ? "fsw " : "fsd ")
                           << tempFloatReg << "," << baseOffset + memberOffset << "(s0)"
                           << std::endl;
                    context.FreeTemporary(tempFloatReg, stream);
                    break;
                }
                case TypeSpecifier::Type::ARRAY: {
                    assert((*it)->IsList() && "Array initializer must be a list");
                    const auto &childInitializerList = static_cast<const InitializerList &>(**it); // NOLINT(*-pro-type-static-cast-downcast)
                    EmitArrayInitializer(stream, context, tempReg, childInitializerList, memberType.GetArrayType(),
                                         baseOffset + memberOffset);
                    break;
                }
                case TypeSpecifier::Type::STRUCT: {
                    assert((*it)->IsList() && "Struct initializer must be a list");
                    const auto &childInitializerList = static_cast<const InitializerList &>(**it); // NOLINT(*-pro-type-static-cast-downcast)
                    EmitStructInitializer(childInitializerList, memberType, tempReg, baseOffset + memberOffset, stream,
                                          context);
                    break;
                }
                case TypeSpecifier::Type::VOID:
                    throw std::runtime_error(
                            "Declaration::EmitRISC() called on an unsupported struct member type");
            }
            it++;
            memberOffset += memberType.GetTypeSize();
        }
    }

    void Declaration::EmitArrayInitializer(std::ostream &stream, Context &context, const Register &tempReg,
                                           const InitializerList &initializerList, const TypeSpecifier &type,
                                           const int &offset) {
        int idx = 0;
        for (const auto &initializer: initializerList) {
            initializer->EmitRISC(stream, context, tempReg);
            switch (type) {
                case TypeSpecifier::INT:
                case TypeSpecifier::UNSIGNED:
                case TypeSpecifier::POINTER:
                case TypeSpecifier::ENUM:
                    stream << "sw " << tempReg << "," << offset + idx * type.GetTypeSize() << "(s0)"
                           << std::endl;
                    break;
                case TypeSpecifier::FLOAT:
                    stream << "fsw " << tempReg << "," << offset + idx * type.GetTypeSize() << "(s0)"
                           << std::endl;
                    break;
                case TypeSpecifier::DOUBLE:
                    stream << "fsd " << tempReg << "," << offset + idx * type.GetTypeSize() << "(s0)"
                           << std::endl;
                    break;
                case TypeSpecifier::CHAR:
                    stream << "sb " << tempReg << "," << offset + idx * type.GetTypeSize() << "(s0)"
                           << std::endl;
                    break;
                case TypeSpecifier::STRUCT: {
                    assert(initializer->IsList() && "Struct initializer must be a list");
                    const auto &structInitializerList = static_cast<const InitializerList &>(*initializer); // NOLINT(*-pro-type-static-cast-downcast)
                    EmitStructInitializer(structInitializerList, type, tempReg, offset + idx * type.GetTypeSize(),
                                          stream, context);
                    break;
                }
                case TypeSpecifier::ARRAY: {
                    assert(initializer->IsList() && "Array initializer must be a list");
                    const auto &arrayInitializerList = static_cast<const InitializerList &>(*initializer); // NOLINT(*-pro-type-static-cast-downcast)
                    EmitArrayInitializer(stream, context, tempReg, arrayInitializerList, type.GetArrayType(),
                                         offset + idx * type.GetTypeSize());
                    break;
                }
                case TypeSpecifier::VOID:
                    throw std::runtime_error(
                            "Declaration::EmitRISC() called on an unsupported array type");
            }
            idx++;
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
