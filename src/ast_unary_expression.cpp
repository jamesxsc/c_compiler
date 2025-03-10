#include "ast_multiplicative_unary_expressions.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    // Lvalue asserts are in GetIdentifier impls
    void UnaryExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // todo remove usage of destreg as a temp/ don't "return" to it if it is zero/unset
        switch (op_) {
            case UnaryOperator::PostfixPromote:
                postfixChild_->EmitRISC(stream, context, destReg);
                break;
            case UnaryOperator::PrefixIncrement:
            case UnaryOperator::PrefixDecrement: {
                // To an extent this assumes child unary is an lvalue
                std::string identifier = unaryChild_->GetIdentifier();
                if (context.IsGlobal(identifier)) {
                    Register tempReg = context.AllocateTemporary();
                    stream << "lui " << destReg << ",%hi(" << identifier << ")" << std::endl;
                    stream << "lw " << destReg << ",%lo(" << identifier << ")(" << destReg << ")" << std::endl;
                    stream << "addi " << tempReg << "," << destReg << ","
                           << (op_ == UnaryOperator::PrefixIncrement ? "1" : "-1") << std::endl;
                    stream << "lui " << destReg << ",%hi(" << identifier << ")" << std::endl;
                    stream << "sw " << tempReg << ",%lo(" << identifier << ")(" << destReg << ")" << std::endl;
                    context.FreeTemporary(tempReg);
                    // Store the incremented/decremented value in the destination register
                    unaryChild_->EmitRISC(stream, context, destReg);
                } else {
                    Variable var = context.CurrentFrame().bindings.Get(identifier);
                    stream << "lw " << destReg << "," << var.offset << "(s0)" << std::endl;
                    stream << "addi " << destReg << "," << destReg << ","
                           << (op_ == UnaryOperator::PrefixIncrement ? "1" : "-1") << std::endl;
                    stream << "sw " << destReg << "," << var.offset << "(s0)" << std::endl;
                    // Store the incremented/decremented value in the destination register
                    unaryChild_->EmitRISC(stream, context, destReg);
                }
                break;
            }
                // Below all multiplicative child expression
            case UnaryOperator::AddressOf: {
                std::string identifier = multiplicativeChild_->GetIdentifier();
                if (context.IsGlobal(identifier)) {
                    stream << "lui " << destReg << ",%hi(" << identifier << ")" << std::endl;
                    stream << "addi " << destReg << "," << destReg << ",%lo(" << identifier << ")" << std::endl;
                } else {
                    Variable atAddress = context.CurrentFrame().bindings.Get(identifier);
                    stream << "addi " << destReg << ",s0," << atAddress.offset << std::endl;
                }
                break;
            }
            case UnaryOperator::Dereference: {
                std::string identifier = multiplicativeChild_->GetIdentifier();
                // todo maybe the first load should be delegated to child
                if (context.IsGlobal(identifier)) {
                    stream << "lui " << destReg << ",%hi(" << identifier << ")" << std::endl;
                    stream << "lw " << destReg << ",%lo(" << identifier << ")(" << destReg << ")" << std::endl;
                    stream << "lw " << destReg << ",0(" << destReg << ")" << std::endl;
                } else {
                    // Variable is a pointer - get value at address in variable
                    // This should only be called for RHS
                    Variable ptr = context.CurrentFrame().bindings.Get(identifier);
                    // Reuse destReg rather than waste a temporary
                    stream << "lw " << destReg << "," << ptr.offset << "(s0)" << std::endl;
                    stream << "lw " << destReg << ",0(" << destReg << ")" << std::endl;
                }
                break;
            }
            case UnaryOperator::Plus:
                // Does nothing
                multiplicativeChild_->EmitRISC(stream, context, destReg);
                break;
            case UnaryOperator::Minus:
                multiplicativeChild_->EmitRISC(stream, context, destReg);
                stream << "neg " << destReg << "," << destReg << std::endl;
                break;
            case UnaryOperator::BitwiseNot:
                multiplicativeChild_->EmitRISC(stream, context, destReg);
                stream << "not " << destReg << "," << destReg << std::endl;
                break;
            case UnaryOperator::LogicalNot:
                multiplicativeChild_->EmitRISC(stream, context, destReg);
                stream << "seqz " << destReg << "," << destReg << std::endl;
                stream << "andi " << destReg << "," << destReg << ",0xff" << std::endl;
                break;
            case UnaryOperator::SizeofUnary:
                stream << "li " << destReg << "," << GetTypeSize(unaryChild_->GetType(context)) << std::endl;
                break;
            case UnaryOperator::SizeofType:
                stream << "li " << destReg << "," << GetTypeSize(typeNameChild_->GetType(context)) << std::endl;
                break;
        }
    }

    void UnaryExpression::Print(std::ostream &stream) const {
        switch (op_) {
            case UnaryOperator::PostfixPromote:
                postfixChild_->Print(stream);
                break;
            case UnaryOperator::PrefixIncrement:
                stream << "++";
                unaryChild_->Print(stream);
                break;
            case UnaryOperator::PrefixDecrement:
                stream << "--";
                unaryChild_->Print(stream);
                break;
            case UnaryOperator::AddressOf:
                stream << "&";
                multiplicativeChild_->Print(stream);
                break;
            case UnaryOperator::Dereference:
                stream << "*";
                multiplicativeChild_->Print(stream);
                break;
            case UnaryOperator::Plus:
                stream << "+";
                multiplicativeChild_->Print(stream);
                break;
            case UnaryOperator::Minus:
                stream << "-";
                multiplicativeChild_->Print(stream);
                break;
            case UnaryOperator::BitwiseNot:
                stream << "~";
                multiplicativeChild_->Print(stream);
                break;
            case UnaryOperator::LogicalNot:
                stream << "!";
                multiplicativeChild_->Print(stream);
                break;
            case UnaryOperator::SizeofUnary:
                stream << "sizeof(";
                unaryChild_->Print(stream);
                stream << ")";
                break;
            case UnaryOperator::SizeofType:
                stream << "sizeof(";
                typeNameChild_->Print(stream);
                stream << ")";
                break;
        }
    }

    // We don't use references since at a lower level there may be a cast leaving a dangling reference
    std::string UnaryExpression::GetIdentifier() const {
        switch (op_) {
            case UnaryOperator::PostfixPromote:
                return postfixChild_->GetIdentifier();
            case UnaryOperator::PrefixIncrement:
            case UnaryOperator::PrefixDecrement:
            case UnaryOperator::SizeofUnary:
            case UnaryOperator::SizeofType:
                return unaryChild_->GetIdentifier();
            case UnaryOperator::Dereference: // This can be called on lhs or rhs of assignment, should work the same in both
            case UnaryOperator::AddressOf:
            case UnaryOperator::Plus:
            case UnaryOperator::Minus:
            case UnaryOperator::BitwiseNot:
            case UnaryOperator::LogicalNot:
                return multiplicativeChild_->GetIdentifier();
        }
        std::cerr << "Invalid unary operator" << std::endl;
        exit(1);
    }

    TypeSpecifier UnaryExpression::GetType(Context &context) const {
        switch (op_) {
            case UnaryOperator::PostfixPromote:
                return postfixChild_->GetType(context);
            case UnaryOperator::PrefixIncrement:
            case UnaryOperator::PrefixDecrement:
                return unaryChild_->GetType(context);
            case UnaryOperator::AddressOf:
            case UnaryOperator::Dereference:
                return TypeSpecifier::POINTER; // todo ptrs addressof is probably wrong if ever called
            case UnaryOperator::Plus:
            case UnaryOperator::Minus:
            case UnaryOperator::BitwiseNot:
            case UnaryOperator::LogicalNot:
                return multiplicativeChild_->GetType(context);
            case UnaryOperator::SizeofUnary:
            case UnaryOperator::SizeofType:
                return TypeSpecifier::INT; // todo size_t or unsigned?
        }
        std::cerr << "Invalid unary operator" << std::endl;
        exit(1);
    }

    bool UnaryExpression::ContainsFunctionCall() const {
        switch (op_) {
            case UnaryOperator::PostfixPromote:
                return postfixChild_->ContainsFunctionCall();
            case UnaryOperator::PrefixIncrement:
            case UnaryOperator::PrefixDecrement:
                return unaryChild_->ContainsFunctionCall();
            case UnaryOperator::AddressOf:
            case UnaryOperator::Dereference:
            case UnaryOperator::Plus:
            case UnaryOperator::Minus:
            case UnaryOperator::BitwiseNot:
            case UnaryOperator::LogicalNot:
                return multiplicativeChild_->ContainsFunctionCall();
            case UnaryOperator::SizeofUnary:
            case UnaryOperator::SizeofType:
                return false;
        }
        std::cerr << "Invalid unary operator" << std::endl;
        exit(1);
    }

    // These are where it gets complicated because for pointers we need the variable being &'d (can't be *'d or anything else)
    int UnaryExpression::GetGlobalValue() const {
        if (op_ == UnaryOperator::PostfixPromote) {
            return postfixChild_->GetGlobalValue();
        } else {
            throw std::runtime_error("UnaryExpression::GetGlobalValue() called on a non constant");
        }
    }

    std::string UnaryExpression::GetGlobalIdentifier() const {
        // For &x both branches will be called
        if (op_ == UnaryOperator::AddressOf) {
            return multiplicativeChild_->GetGlobalIdentifier();
        } else if (op_ == UnaryOperator::PostfixPromote) {
            return postfixChild_->GetGlobalIdentifier();
        } else {
            throw std::runtime_error("UnaryExpression::GetGlobalIdentifier() called on a non &'d global");
        }
    }

} // namespace ast
