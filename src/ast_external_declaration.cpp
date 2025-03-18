#include <cassert>
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
                stream << ".globl " << identifier << std::endl;
                stream << (type == TypeSpecifier::DOUBLE ? ".align 3" : ".align 2") << std::endl;
                // TODO check directives and what is required for our project
                stream << ".data" << std::endl; // .sdata may be more appropriate (GCC)
                if (initDeclarator->HasInitializer()) {
                    if (initDeclarator->IsArray()) {
                        assert(initDeclarator->GetInitializer().IsList() && "Array initializer must be a list");
                        context.InsertGlobal(identifier, initDeclarator->BuildArray(type,context).type);
                        const auto &initializerList = static_cast<const InitializerList &>(initDeclarator->GetInitializer()); // NOLINT(*-pro-type-static-cast-downcast)
                        stream << ".size " << identifier << "," << context.GetGlobalType(identifier).GetTypeSize()
                               << std::endl;
                        stream << identifier << ":" << std::endl;
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
                                case TypeSpecifier::VOID:
                                case TypeSpecifier::STRUCT: // todo arrays of structs
                                case TypeSpecifier::ARRAY: // todo multidim
                                    throw std::runtime_error(
                                            "ExternalDeclaration::EmitRISC() called on an unsupported array type");
                            }
                        }
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
                                auto it = initializerList.begin();
                                for (const auto &member: type.GetStructMembers()) {
                                    if (member.first == "#padding") {
                                        stream << ".zero " << member.second.GetTypeSize() << std::endl;
                                        continue;
                                    }
                                    // todo ideally change the align directive

                                    TypeSpecifier memberType = member.second;
                                    switch (memberType) {
                                        case TypeSpecifier::Type::INT:
                                        case TypeSpecifier::Type::UNSIGNED:
                                        case TypeSpecifier::Type::POINTER:
                                        case TypeSpecifier::Type::ENUM:
                                            stream << ".word " << (*it)->Evaluate<int>(context) << std::endl;
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
                                        case TypeSpecifier::Type::ARRAY:
                                        case TypeSpecifier::Type::STRUCT:
                                        case TypeSpecifier::Type::VOID:
                                            throw std::runtime_error(
                                                    "ExternalDeclaration::EmitRISC() called on an unsupported global struct member type");
                                    }
                                    it++;
                                }
                                break;
                            }
                            case TypeSpecifier::ARRAY: // Should never get here
                            case TypeSpecifier::VOID:
                                throw std::runtime_error(
                                        "ArrayIndexExpression::EmitRISC() called on an unsupported array type");
                        }
                    }
                } else {
                    if (initDeclarator->IsArray()) {
                        TypeSpecifier array = initDeclarator->BuildArray(type, context).type;
                        context.InsertGlobal(identifier, array); // Should be safe to copy for rvalue
                        stream << ".size " << identifier << "," << array.GetTypeSize() << std::endl;
                        stream << identifier << ":" << std::endl;
                        stream << ".zero " << array.GetTypeSize() << std::endl;
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

    void ExternalDeclaration::Print(std::ostream &stream) const {
        declarationSpecifiers_->Print(stream);
        initDeclaratorList_->Print(stream);
    }

}
