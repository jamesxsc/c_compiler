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
                context.InsertFunction(initDeclarator->GetIdentifier(),
                                       initDeclarator->BuildFunction(declarationSpecifiers_->GetType(context), context));
            } else {
                // Handle global variables
                std::string identifier = initDeclarator->GetIdentifier();
                TypeSpecifier type = declarationSpecifiers_->GetType(context);
                if (initDeclarator->IsPointer())
                    type = TypeSpecifier(TypeSpecifier::POINTER, type);
                stream << ".globl " << identifier << std::endl;
                stream << ".align 2" << std::endl; // TODO check directives and what is required for our project
                stream << ".data" << std::endl; // .sdata may be more appropriate (GCC)
                if (initDeclarator->HasInitializer()) {
                    if (initDeclarator->IsArray()) {
                        assert(initDeclarator->GetInitializer().IsList() && "Array initializer must be a list");
                        context.InsertGlobal(identifier, initDeclarator->BuildArray(type, context).type); // Pretty much discard the offset etc info.
                        const auto& initializerList = static_cast<const InitializerList&>(initDeclarator->GetInitializer()); // NOLINT(*-pro-type-static-cast-downcast)
                        stream << ".size " << identifier << "," << context.GetGlobalType(identifier).GetTypeSize() << std::endl;
                        stream << identifier << ":" << std::endl;
                        for (const auto& initializer : initializerList) {
                            switch (type) {
                                case TypeSpecifier::INT:
                                case TypeSpecifier::UNSIGNED:
                                    stream << ".word " << initializer->GetGlobalValue() << std::endl;
                                    break;
                                case TypeSpecifier::CHAR:
                                    stream << ".byte " << initializer->GetGlobalValue() << std::endl;
                                    break;
                                case TypeSpecifier::POINTER:
                                    stream << ".word " << initializer->GetGlobalIdentifier() << std::endl;
                                    break;
                                case TypeSpecifier::FLOAT:
                                case TypeSpecifier::DOUBLE:
                                case TypeSpecifier::VOID:
                                case TypeSpecifier::ENUM:
                                case TypeSpecifier::STRUCT:
                                case TypeSpecifier::ARRAY:
                                    throw std::runtime_error(
                                            "ExternalDeclaration::EmitRISC() called on an unsupported array type");
                                    // todo handle these
                            }
                        }
                    } else {
                        context.InsertGlobal(identifier, type);
                        stream << ".size " << identifier << "," << type.GetTypeSize() << std::endl;
                        stream << identifier << ":" << std::endl;
                        switch (type) {
                            case TypeSpecifier::INT:
                            case TypeSpecifier::UNSIGNED:
                                // Can only be a constant (can't be assigned to another global for example)
                                stream << ".word " << initDeclarator->GetGlobalInitializerValue() << std::endl;
                                break;
                            case TypeSpecifier::CHAR:
                                stream << ".byte " << initDeclarator->GetGlobalInitializerValue() << std::endl;
                                break;
                            case TypeSpecifier::POINTER:
                                // Simply .word (RHS identifier)
                                stream << ".word " << initDeclarator->GetGlobalInitializerIdentifier() << std::endl;
                                break;
                                // TODO float work here and above, use .float/.double but a third global method needs adding
                            case TypeSpecifier::FLOAT:
                            case TypeSpecifier::DOUBLE:
                            case TypeSpecifier::VOID:
                            case TypeSpecifier::ENUM:
                            case TypeSpecifier::STRUCT:
                            case TypeSpecifier::ARRAY:
                                throw std::runtime_error(
                                        "ArrayIndexExpression::EmitRISC() called on an unsupported array type");
                                // todo handle these
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
