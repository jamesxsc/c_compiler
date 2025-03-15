#include "ast_expression.hpp"
#include "ast_type_specifier.hpp"

// Note that this feature isn't technically required
namespace ast {
    void ast::Expression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (first_)
            first_->EmitRISC(stream, context, destReg);

        assignment_->EmitRISC(stream, context, destReg);
    }

    void ast::Expression::Print(std::ostream &stream) const {
        if (first_) {
            first_->Print(stream);
            stream << ", ";
        }
        assignment_->Print(stream);
    }

    TypeSpecifier Expression::GetType(Context& context) const {
        return assignment_->GetType(context); // Return type of last comma separated expression
    }

    bool Expression::ContainsFunctionCall() const {
        return (first_ != nullptr && first_->ContainsFunctionCall()) || assignment_->ContainsFunctionCall();
    }

    std::string Expression::GetGlobalIdentifier() const {
        return assignment_->GetGlobalIdentifier();
    }

    int Expression::GetGlobalValue() const {
        return assignment_->GetGlobalValue();
    }

}
