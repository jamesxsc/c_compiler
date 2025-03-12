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
                                       initDeclarator->BuildFunction(GetType(context), context));
            } else {
                // Handle global variables
                std::string identifier = initDeclarator->GetIdentifier();
                TypeSpecifier type = GetType(context);
                stream << ".globl " << identifier << std::endl;
                stream << ".align 2" << std::endl; // TODO check directives and what is required for our project
                stream << ".data" << std::endl; // .sdata may be more appropriate (GCC)
                if (initDeclarator->HasInitializer()) {
                    if (initDeclarator->IsArray()) {
                        assert(initDeclarator->GetInitializer().IsList() && "Array initializer must be a list");
                        context.InsertGlobalArray(identifier, initDeclarator->BuildArray(type, context));
                        const auto& initializerList = static_cast<const InitializerList&>(initDeclarator->GetInitializer()); // NOLINT(*-pro-type-static-cast-downcast)
                        stream << ".size " << identifier << "," << context.GetGlobalArray(identifier).size << std::endl;
                        stream << identifier << ":" << std::endl;
                        for (const auto& initializer : initializerList) {
                            switch (type) {
                                case TypeSpecifier::INT:
                                    stream << ".word " << initializer->GetGlobalValue() << std::endl;
                                    break;
                                case TypeSpecifier::CHAR:
                                    stream << ".byte " << initializer->GetGlobalValue() << std::endl;
                                    break;
                                case TypeSpecifier::POINTER:
                                    stream << ".word " << initializer->GetGlobalIdentifier() << std::endl;
                                    break;
                                case TypeSpecifier::FLOAT:
                                    break;
                                case TypeSpecifier::DOUBLE:
                                    break;
                            }
                        }
                    } else {
                        context.InsertGlobal(identifier, type);
                        stream << ".size " << identifier << "," << GetTypeSize(type) << std::endl;
                        stream << identifier << ":" << std::endl;
                        switch (type) {
                            case TypeSpecifier::INT:
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
                                // TODO float work here and above, note double should be given two words or can we use .float? is there a .double cx the other todo relating to this (constant)
                            case TypeSpecifier::FLOAT:
                                break;
                            case TypeSpecifier::DOUBLE:
                                break;
                        }
                    }
                } else {
                    if (initDeclarator->IsArray()) {
                        Array array = initDeclarator->BuildArray(type, context);
                        context.InsertGlobalArray(identifier, array); // Should be safe to copy for rvalue
                        stream << ".size " << identifier << "," << array.size << std::endl;
                        stream << identifier << ":" << std::endl;
                        stream << ".zero " << array.size << std::endl;
                    } else {
                        context.InsertGlobal(identifier, type);
                        stream << ".size " << identifier << "," << GetTypeSize(type) << std::endl;
                        stream << identifier << ":" << std::endl;
                        stream << ".zero " << GetTypeSize(type) << std::endl;
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
