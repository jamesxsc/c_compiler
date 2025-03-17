#include <cassert>
#include <cmath>
#include "ast_postfix_expression.hpp"
#include "ast_identifier.hpp"
#include "ast_type_specifier.hpp"
#include "ast_array_index_expression.hpp"
#include "risc_utils.hpp"

namespace ast {

    void PostfixExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        switch (op_) {
            case PostfixOperator::PrimaryPromote:
            case PostfixOperator::FunctionCallPromote:
            case PostfixOperator::ArrayIndexPromote:
                child_->EmitRISC(stream, context, destReg);
                break;
            case PostfixOperator::PostfixIncrement:
            case PostfixOperator::PostfixDecrement:
                Utils::EmitIncrementDecrement(stream, context, destReg, *child_, op_ == PostfixOperator::PostfixDecrement, true);
                break;
        }
    }

    void PostfixExpression::Print(std::ostream &stream) const {
        switch (op_) {
            case PostfixOperator::PrimaryPromote:
            case PostfixOperator::FunctionCallPromote:
            case PostfixOperator::ArrayIndexPromote:
                child_->Print(stream);
                break;
            case PostfixOperator::PostfixIncrement:
                child_->Print(stream);
                stream << "++";
                break;
            case PostfixOperator::PostfixDecrement:
                child_->Print(stream);
                stream << "--";
                break;
        }
    }

    TypeSpecifier PostfixExpression::GetType(Context &context) const {
        return child_->GetType(context);
    }

    // todo look to deprecate this
    std::string PostfixExpression::GetIdentifier() const {
        switch (op_) {
            case PostfixOperator::PrimaryPromote:
            case PostfixOperator::FunctionCallPromote: {
                Identifier identifier = dynamic_cast<const Identifier &>(*child_); // todo that also makes this wrong for primarypromote
                return identifier.GetIdentifier();
            }
            case PostfixOperator::PostfixIncrement:
            case PostfixOperator::PostfixDecrement: {
                // Child is a postfix expression / todo this assumption is wrong (it may be primarypromote underlying), needs to be passed along the whole chain, or we commit to using raw addresses, i think i prefer that, and mark this unsupported
                return dynamic_cast<const PostfixExpression *>(child_.get())->GetIdentifier();
            }
            case PostfixOperator::ArrayIndexPromote:
                return dynamic_cast<const ArrayIndexExpression *>(child_.get())->GetIdentifier();
        }

        Identifier identifier = dynamic_cast<const Identifier &>(*child_);
        return identifier.GetIdentifier();
    }

    bool PostfixExpression::ContainsFunctionCall() const {
        return child_->ContainsFunctionCall();
    }

    std::string PostfixExpression::GetGlobalIdentifier() const {
        return child_->GetGlobalIdentifier();
    }

    const Expression &PostfixExpression::GetArrayIndexExpression() const {
        assert(op_ == PostfixOperator::ArrayIndexPromote &&
               "PostfixExpression::GetArrayIndexExpression called on non-array");
        return dynamic_cast<const ArrayIndexExpression *>(child_.get())->GetIndexExpression();
    }

    int PostfixExpression::Evaluate(Context &context) const {
        if (op_ != PostfixOperator::PrimaryPromote)
            throw std::runtime_error("PostfixExpression::Evaluate() called on non-primary expression");

        return child_->Evaluate(context);
    }

    double PostfixExpression::EvaluateFloat(ast::Context &context) const {
        if (op_ != PostfixOperator::PrimaryPromote)
            throw std::runtime_error("PostfixExpression::EvaluateFloat() called on non-primary expression");

        return child_->EvaluateFloat(context);
    }

} // namespace ast
