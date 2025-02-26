#include "ast_declarator.hpp"

namespace ast {

    // TODO unclear if this will ever have to do anything, or if everything is in "init declarator" and above - probably the latter
    // note isn't called at the moment but would be called by init declarator
    void Declarator::Print(std::ostream &stream) const {
        if (isDirect_)
            return; // Nothing to do! This is part of a function or array
    }

    void Declarator::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (isDirect_)
            return; // Nothing to do! This is part of a function or array
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

}
