#include "ast_declarator.hpp"

namespace ast {

Declarator::Declarator(std::string identifier, bool isDirect)
    : identifier_(std::move(identifier))
    , isDirect_(isDirect)
    , typeSpecifier_(TypeSpecifier::INT)
    , hasInitializer_(false)
    , initializer_(nullptr)
{
}

void Declarator::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
    if (isDirect_)
        return;
}

void Declarator::Print(std::ostream &stream) const {
    if (isDirect_)
        return;
    stream << " " << identifier_;
}

const std::string &Declarator::GetIdentifier() const {
    return identifier_;
}

bool Declarator::IsDirect() const {
    return isDirect_;
}

void Declarator::Indirect() {
    isDirect_ = false;
}

void Declarator::Direct() {
    isDirect_ = true;
}

Type Declarator::GetType(Context &context) const {
    return Type(typeSpecifier_);
}

void Declarator::SetTypeSpecifier(TypeSpecifier spec) {
    typeSpecifier_ = spec;
}

TypeSpecifier Declarator::GetTypeSpecifier() const {
    return typeSpecifier_;
}

void Declarator::SetHasInitializer(bool h) {
    hasInitializer_ = h;
}

bool Declarator::HasInitializer() const {
    return hasInitializer_;
}

void Declarator::SetInitializer(Initializer* init) {
    initializer_ = init;
}

Initializer* Declarator::GetInitializer() const {
    return initializer_;
}

bool Declarator::IsFunction() const {
    return false;
}

} // namespace ast
