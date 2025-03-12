#include "ast_external_declaration.hpp"

namespace ast {

    void ExternalDeclaration::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        for (const auto &initDeclarator: *initDeclaratorList_) {
            // TODO delete my notes and put anything useful in management/readme
            // Could add a base stack frame, or a separate instance of Bindings
            // Then we need to update where they're accessed.
            // we're gonna have to search for all uses of bindings.Get
            // InsertOrOverwrite is only used for declarations. IsGlobal needs to check its not shadowed if there is a frame on the stack
            // IsGlobal done
            // Identifier done, Postfix, Unary, Assignment
            // Its widely used e.g. Unary operators. so create another function to obtain (if we don't guarantee a frame existing)
            // I think find them in a single context function but return in struct an isglobal? since codegen is different - no because we don't even need to store offset etc.
            // IsGlobal isn't pretty but might help
            // the other option is push fw declarations and globals to translation unit and don't call this... tempting to avoid bloating it or have an external declaration type - preferred IMO since it doesn't make sense to only sometimes call Declaration::EmitRISC
            // this works, then we still have to sort out accessing though

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
                        // todo waiting, see declaration
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
                                // TODO float work here, note double should be given two words or can we use .float? is there a .double cx the other todo relating to this (constant)
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
