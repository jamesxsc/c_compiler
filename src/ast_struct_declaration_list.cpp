#include "ast_struct_declaration_list.hpp"

namespace ast {

    void ast::StructDeclarationList::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Handled above this layer
    }

    void ast::StructDeclarationList::Print(std::ostream &stream) const {
        for (const auto &declaration: declarations_) {
            declaration->Print(stream);
        }
    }

    void ast::StructDeclarationList::PushBack(StructDeclarationPtr declaration) {
        declarations_.push_back(std::move(declaration));
    }

    const std::vector<StructDeclarationPtr> &ast::StructDeclarationList::GetDeclarations() const {
        return declarations_;
    }

    std::vector<std::pair<std::string, TypeSpecifier>> ast::StructDeclarationList::GetMembers(Context &context) const {
        std::vector<std::pair<std::string, TypeSpecifier>> members;
        for (const auto &declaration: declarations_) {
            TypeSpecifier type = declaration->GetType(context);
            for (const auto &declarator: declaration->GetDeclarators()) {
                members.emplace_back(declarator->GetIdentifier(), declaration->GetType(context));
            }
        }
        return members;
    }
}
