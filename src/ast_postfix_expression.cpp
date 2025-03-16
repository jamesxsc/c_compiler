#include <cassert>
#include "ast_postfix_expression.hpp"
#include "ast_identifier.hpp"
#include "ast_type_specifier.hpp"
#include "ast_array_index_expression.hpp"
#include "ast_assignment_expression.hpp"
#include "ast_multiplicative_unary_expressions.hpp"

namespace ast {

void PostfixExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
    switch (op_) {
        case PostfixOperator::PrimaryPromote:
        case PostfixOperator::FunctionCallPromote:
        case PostfixOperator::ArrayIndexPromote:
            child_->EmitRISC(stream, context, destReg);
            break;

        case PostfixOperator::PostfixIncrement:
        case PostfixOperator::PostfixDecrement: {
            // If child_ is array indexing or pointer dereference, handle pointer-based logic.
            auto arrayIndex = dynamic_cast<const ArrayIndexExpression*>(child_.get());
            if (arrayIndex || child_->IsPointerDereference()) {
                Register addrReg = context.AllocateTemporary();
                if (arrayIndex) {
                    // If it's array indexing, get the address from EmitAddressRISC
                    addrReg = arrayIndex->EmitAddressRISC(stream, context);
                } else {
                    // Otherwise, it's pointer dereference with a named variable or <no-identifier>
                    std::string identifier = GetIdentifier();
                    if (context.IsGlobal(identifier)) {
                        Register tmp = context.AllocateTemporary();
                        stream << "lui " << tmp << ",%hi(" << identifier << ")" << std::endl;
                        stream << "lw " << tmp << ",%lo(" << identifier << ")(" << tmp << ")" << std::endl;
                        stream << "mv " << addrReg << "," << tmp << std::endl;
                        context.FreeTemporary(tmp);
                    } else {
                        // If identifier is "<no-identifier>", skip
                        if (identifier == "<no-identifier>") {
                            throw std::runtime_error(
                                "Postfix inc/dec on pointer expression with no named variable");
                        }
                        Variable var = context.CurrentFrame().bindings.Get(identifier);
                        stream << "lw " << addrReg << "," << var.offset << "(s0)" << std::endl;
                    }
                }
                // Perform the load, inc/dec, store
                Register oldVal = context.AllocateTemporary();
                stream << "lw " << oldVal << ",0(" << addrReg << ")" << std::endl;
                stream << "addi " << oldVal << "," << oldVal << ","
                       << (op_ == PostfixOperator::PostfixIncrement ? "1" : "-1") << std::endl;
                stream << "sw " << oldVal << ",0(" << addrReg << ")" << std::endl;
                if (destReg != Register::zero) {
                    // Postfix means return the old value; minimal approach
                    stream << "addi " << oldVal << "," << oldVal << ","
                           << (op_ == PostfixOperator::PostfixIncrement ? "-1" : "1") << std::endl;
                    stream << "mv " << destReg << "," << oldVal << std::endl;
                }
                context.FreeTemporary(oldVal);
                context.FreeTemporary(addrReg);
            } else {
                // If child_ is a named variable
                std::string identifier = GetIdentifier();
                if (identifier == "<no-identifier>") {
                    throw std::runtime_error(
                        "Postfix inc/dec on non-variable expression with no named identifier");
                }
                if (context.IsGlobal(identifier)) {
                    if (destReg != Register::zero) {
                        child_->EmitRISC(stream, context, destReg);
                    }
                    Register tempReg = context.AllocateTemporary();
                    Register tempReg2 = context.AllocateTemporary();
                    stream << "lui " << tempReg2 << ",%hi(" << identifier << ")" << std::endl;
                    stream << "lw " << tempReg2 << ",%lo(" << identifier << ")(" << tempReg2 << ")" << std::endl;
                    stream << "addi " << tempReg << "," << tempReg2 << ","
                           << (op_ == PostfixOperator::PostfixIncrement ? "1" : "-1") << std::endl;
                    stream << "lui " << tempReg2 << ",%hi(" << identifier << ")" << std::endl;
                    stream << "sw " << tempReg << ",%lo(" << identifier << ")(" << tempReg2 << ")" << std::endl;
                    context.FreeTemporary(tempReg);
                    context.FreeTemporary(tempReg2);
                } else {
                    // Local named variable
                    Variable variable = context.CurrentFrame().bindings.Get(identifier);
                    if (destReg != Register::zero) {
                        child_->EmitRISC(stream, context, destReg);
                    }
                    Register tempReg = context.AllocateTemporary();
                    stream << "lw " << tempReg << "," << variable.offset << "(s0)" << std::endl;
                    stream << "addi " << tempReg << "," << tempReg << ","
                           << (op_ == PostfixOperator::PostfixIncrement ? 1 : -1) << std::endl;
                    stream << "sw " << tempReg << "," << variable.offset << "(s0)" << std::endl;
                    context.FreeTemporary(tempReg);
                }
            }
            break;
        }
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

<<<<<<< HEAD
TypeSpecifier PostfixExpression::GetType(Context &context) const {
    return child_->GetType(context);
}

std::string PostfixExpression::GetIdentifier() const {
    switch (op_) {
        case PostfixOperator::PrimaryPromote:
        case PostfixOperator::FunctionCallPromote: {
            if (auto ident = dynamic_cast<const Identifier*>(child_.get())) {
                return ident->GetIdentifier();
            }
            if (auto unary = dynamic_cast<const UnaryExpression*>(child_.get())) {
                return unary->GetIdentifier(); 
            }
            return "<no-identifier>"; 
        }
        case PostfixOperator::PostfixIncrement:
        case PostfixOperator::PostfixDecrement: {
            if (auto post = dynamic_cast<const PostfixExpression*>(child_.get())) {
                return post->GetIdentifier();
            }
            if (auto unary = dynamic_cast<const UnaryExpression*>(child_.get())) {
                return unary->GetIdentifier();
            }
            return "<no-identifier>";
        }
        case PostfixOperator::ArrayIndexPromote: {
            if (auto arrIdx = dynamic_cast<const ArrayIndexExpression*>(child_.get())) {
                return arrIdx->GetIdentifier();
            }
            return "<no-identifier>";
        }
    }
    return "<unreachable>";
}

bool PostfixExpression::ContainsFunctionCall() const {
    return child_->ContainsFunctionCall();
}

int PostfixExpression::GetGlobalValue() const {
    return child_->GetGlobalValue();
}

std::string PostfixExpression::GetGlobalIdentifier() const {
    return child_->GetGlobalIdentifier();
}

const Expression& PostfixExpression::GetArrayIndexExpression() const {
    assert(op_ == PostfixOperator::ArrayIndexPromote);
    auto arrIdx = dynamic_cast<const ArrayIndexExpression*>(child_.get());
    if (!arrIdx) {
        throw std::runtime_error("GetArrayIndexExpression() called but child is not ArrayIndexExpression");
=======
    std::string PostfixExpression::GetGlobalIdentifier() const {
        return child_->GetGlobalIdentifier();
>>>>>>> aaabd7d9f74208ee579772452eb7f2b51bfa0e1d
    }
    return arrIdx->GetIndexExpression();
}

int PostfixExpression::Evaluate() const {
    if (op_ != PostfixOperator::PrimaryPromote) {
        throw std::runtime_error("PostfixExpression::Evaluate() called on non-primary expression");
    }
    return child_->Evaluate();
}

<<<<<<< HEAD
} 
=======
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
>>>>>>> aaabd7d9f74208ee579772452eb7f2b51bfa0e1d
