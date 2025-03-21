#include <cassert>
#include <cmath>
#include "ast_external_declaration.hpp"
#include "ast_initializer_list.hpp"

namespace ast {

    void ExternalDeclaration::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (IsTypedef()) return;

        for (const auto &initDeclarator: *initDeclaratorList_) {
            // Handle forward declarations
            if (initDeclarator->IsFunction()) {
                // Store forward declarations
                TypeSpecifier type = declarationSpecifiers_->GetType(context);
                if (initDeclarator->IsPointer())
                    type = TypeSpecifier(TypeSpecifier::POINTER, type);
                context.InsertFunction(initDeclarator->GetIdentifier(), initDeclarator->BuildFunction(type, context));
            } else {
                // Handle global variables
                std::string identifier = initDeclarator->GetIdentifier();
                TypeSpecifier type = declarationSpecifiers_->GetType(context);
                if (initDeclarator->IsPointer())
                    type = TypeSpecifier(TypeSpecifier::POINTER, type);
                if (initDeclarator->IsArray())
                    type = initDeclarator->BuildArray(type, context).type;

                stream << ".globl " << identifier << std::endl;
                int alignment = static_cast<int>(std::log2(type.GetAlignment()));
                stream << ".align " << alignment << std::endl;
                // TODO check directives and what is required for our project
                stream << ".data" << std::endl; // .sdata may be more appropriate (GCC)
                if (initDeclarator->HasInitializer()) {
                    if (type.IsArray()) {
                        assert(initDeclarator->GetInitializer().IsList() && "Array initializer must be a list");
                        context.InsertGlobal(identifier, type);
                        const auto &initializerList = static_cast<const InitializerList &>(initDeclarator->GetInitializer()); // NOLINT(*-pro-type-static-cast-downcast)
                        stream << ".size " << identifier << "," << context.GetGlobalType(identifier).GetTypeSize()
                               << std::endl;
                        stream << identifier << ":" << std::endl;
                        EmitArrayInitializer(stream, context, initializerList, type.GetArrayType());
                    } else {
                        context.InsertGlobal(identifier, type);
                        stream << ".size " << identifier << "," << type.GetTypeSize() << std::endl;
                        stream << identifier << ":" << std::endl;
                        switch (type) {
                            case TypeSpecifier::INT:
                            case TypeSpecifier::UNSIGNED:
                            case TypeSpecifier::ENUM:
                                // Can only be a constant (can't be assigned to another global for example)
                                stream << ".word " << initDeclarator->EvaluateInitializer<int>(context) << std::endl;
                                break;
                            case TypeSpecifier::POINTER:
                                stream << ".word " << initDeclarator->GetGlobalInitializerIdentifier() << std::endl;
                                break;
                            case TypeSpecifier::CHAR:
                                stream << ".byte " << initDeclarator->EvaluateInitializer<int>(context) << std::endl;
                                break;
                            case TypeSpecifier::FLOAT:
                                stream << ".float " << initDeclarator->EvaluateInitializer<double>(context)
                                       << std::endl;
                                break;
                            case TypeSpecifier::DOUBLE:
                                stream << ".double " << initDeclarator->EvaluateInitializer<double>(context)
                                       << std::endl;
                                break;
                            case TypeSpecifier::STRUCT: {
                                assert(initDeclarator->GetInitializer().IsList() &&
                                       "Struct initializer must be a list");
                                const auto &initializerList = static_cast<const InitializerList &>(initDeclarator->GetInitializer()); // NOLINT(*-pro-type-static-cast-downcast)
                                EmitStructInitializer(stream, context, initializerList, type);
                                break;
                            }
                            case TypeSpecifier::ARRAY: // Should never get here
                            case TypeSpecifier::VOID:
                                throw std::runtime_error(
                                        "ArrayIndexExpression::EmitRISC() called on an unsupported array type");
                        }
                    }
                } else {
                    if (type.IsArray()) {
                        context.InsertGlobal(identifier, type); // Should be safe to copy for rvalue
                        stream << ".size " << identifier << "," << type.GetTypeSize() << std::endl;
                        stream << identifier << ":" << std::endl;
                        stream << ".zero " << type.GetTypeSize() << std::endl;
                    } else {
                        context.InsertGlobal(identifier, type);
                        stream << ".size " << identifier << "," << type.GetTypeSize() << std::endl;
                        stream << identifier << ":" << std::endl;
                        stream << ".zero " << type.GetTypeSize() << std::endl;
                    }
                }
            }
        }
    }

    void ExternalDeclaration::EmitStructInitializer(std::ostream &stream, Context &context, const InitializerList &initializerList, const TypeSpecifier &type) {
        auto it = initializerList.begin();
        for (const auto &member: type.GetStructMembers()) {
            if (member.first == "#padding") {
                stream << ".zero " << member.second.GetTypeSize() << std::endl;
                continue;
            }

            // Incomplete init list
            if (it == initializerList.end()) {
                stream << ".zero " << member.second.GetTypeSize() << std::endl;
                ++it;
                continue;
            }

            TypeSpecifier memberType = member.second;
            switch (memberType) {
                case TypeSpecifier::Type::INT:
                case TypeSpecifier::Type::UNSIGNED:
                case TypeSpecifier::Type::ENUM:
                    stream << ".word " << (*it)->Evaluate<int>(context) << std::endl;
                    break;
                case TypeSpecifier::Type::POINTER:
                    stream << ".word " << (*it)->GetGlobalIdentifier() << std::endl;
                    break;
                case TypeSpecifier::Type::CHAR:
                    stream << ".byte " << (*it)->Evaluate<int>(context) << std::endl;
                    break;
                case TypeSpecifier::Type::FLOAT:
                    stream << ".float " << (*it)->Evaluate<double>(context) << std::endl;
                    break;
                case TypeSpecifier::Type::DOUBLE:
                    stream << ".double " << (*it)->Evaluate<double>(context) << std::endl;
                    break;
                case TypeSpecifier::Type::STRUCT: {
                    assert((*it)->IsList() && "Struct initializer must be a list");
                    const auto &structInitializerList = static_cast<const InitializerList &>(**it); // NOLINT(*-pro-type-static-cast-downcast)
                    EmitStructInitializer(stream, context, structInitializerList, memberType);
                    break;
                }
                case TypeSpecifier::Type::ARRAY: {
                    assert((*it)->IsList() && "Array initializer must be a list");
                    const auto &arrayInitializerList = static_cast<const InitializerList &>(**it); // NOLINT(*-pro-type-static-cast-downcast)
                    EmitArrayInitializer(stream, context, arrayInitializerList, memberType.GetArrayType());
                    break;
                }
                case TypeSpecifier::Type::VOID:
                    throw std::runtime_error(
                            "ExternalDeclaration::EmitRISC() called on an unsupported global struct member type");
            }
            ++it;
        }
    }

    void ExternalDeclaration::EmitArrayInitializer(std::ostream &stream, Context &context, const InitializerList &initializerList, const TypeSpecifier &type) {
        for (const auto &initializer: initializerList) {
            switch (type) { // Type is element type in array case here
                case TypeSpecifier::INT:
                case TypeSpecifier::UNSIGNED:
                case TypeSpecifier::ENUM:
                    stream << ".word " << initializer->Evaluate<int>(context) << std::endl;
                    break;
                case TypeSpecifier::CHAR:
                    stream << ".byte " << initializer->Evaluate<int>(context) << std::endl;
                    break;
                case TypeSpecifier::POINTER:
                    stream << ".word " << initializer->GetGlobalIdentifier() << std::endl;
                    break;
                case TypeSpecifier::FLOAT:
                    stream << ".float " << initializer->Evaluate<double>(context) << std::endl;
                    break;
                case TypeSpecifier::DOUBLE:
                    stream << ".double " << initializer->Evaluate<double>(context) << std::endl;
                    break;
                case TypeSpecifier::STRUCT: {
                    assert(initializer->IsList() && "Struct initializer must be a list");
                    const auto &structInitializerList = static_cast<const InitializerList &>(*initializer); // NOLINT(*-pro-type-static-cast-downcast)
                    EmitStructInitializer(stream, context, structInitializerList, type);
                    break;
                }
                case TypeSpecifier::ARRAY: {
                    assert(initializer->IsList() && "Array initializer must be a list");
                    const auto &arrayInitializerList = static_cast<const InitializerList &>(*initializer); // NOLINT(*-pro-type-static-cast-downcast)
                    EmitArrayInitializer(stream, context, arrayInitializerList, type.GetArrayType());
                    break;
                }
                case TypeSpecifier::VOID:
                    throw std::runtime_error(
                            "ExternalDeclaration::EmitRISC() called on an unsupported array type");
            }
        }
    }

    void ExternalDeclaration::Print(std::ostream &stream) const {
        declarationSpecifiers_->Print(stream);
        initDeclaratorList_->Print(stream);
    }

}
