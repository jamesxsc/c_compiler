#include "ast_array_index_expression.hpp"

namespace ast {
    // todo implement stubs
    void ArrayIndexExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {

    }

    void ArrayIndexExpression::Print(std::ostream &stream) const {
        array_->Print(stream);
        stream << "[";
        index_->Print(stream);
        stream << "]";
    }

    bool ArrayIndexExpression::ContainsFunctionCall() const {
        return index_->ContainsFunctionCall();
    }

    TypeSpecifier ArrayIndexExpression::GetType(Context &context) const {
        return array_->GetType(context); // todo where do we differentiate type of array and elements? cx
    }

    std::string ArrayIndexExpression::GetGlobalIdentifier() const {
        return array_->GetGlobalIdentifier();
    }

    int ArrayIndexExpression::GetGlobalValue() const {
        // probably do need to implement this and the above
        throw std::runtime_error("ArrayIndexExpression::GetGlobalValue() called on an array index expression");
    }

}
