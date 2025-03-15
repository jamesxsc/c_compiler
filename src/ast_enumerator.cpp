#include "ast_enumerator.hpp"

namespace ast {

    void Enumerator::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // Nothing to do
    }

    void Enumerator::Print(std::ostream &stream) const {
        stream << identifier_;
        if (value_) {
            stream << " = ";
            value_->Print(stream);
        }
        stream << ", " << std::endl;
    }

    std::string Enumerator::GetIdentifier() const {
        return identifier_;
    }

    bool Enumerator::HasValue() const {
        return value_ != nullptr;
    }

    int Enumerator::GetValue() const {
        return value_->Evaluate();
    }

}
