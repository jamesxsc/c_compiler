#include "ast_initializer.hpp"

namespace ast {

    void Initializer::EmitRISC(std::ostream& stream, Context& context, Register destReg) const {
        if (expression_)
            expression_->EmitRISC(stream, context, destReg);
    }

    void Initializer::Print(std::ostream& stream) const {
        if (expression_)
            expression_->Print(stream);
    }

    std::string Initializer::GetGlobalIdentifier() const {
        return expression_->GetGlobalIdentifier();
    }

    template<class T>
    T Initializer::Evaluate(Context& context) const {
        throw std::runtime_error("Initializer::Evaluate() called on an unsupported type");
    }

    template<>
    int Initializer::Evaluate<int>(Context &context) const {
        return expression_->Evaluate(context);
    }

    template<>
    double Initializer::Evaluate<double>(Context &context) const {
        return expression_->EvaluateFloat(context);
    }

    bool Initializer::IsList() const {
        return false;
    }

    void Initializer::AddInitializer(InitializerPtr initializer) {
        throw std::runtime_error("Initializer::AddInitializer() called on a non-list initializer");
    }

}
