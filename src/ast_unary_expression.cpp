#include "ast_multiplicative_unary_expressions.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    // Lvalue asserts are in GetIdentifier impls
    void UnaryExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        switch (op_) {
            case UnaryOperator::PostfixPromote:
                postfixChild_->EmitRISC(stream, context, destReg);
                break;
            case UnaryOperator::PrefixIncrement: {
                Variable var = context.CurrentFrame().bindings.at(unaryChild_->GetIdentifier());
                Register reg = context.AllocateTemporary();
                stream << "lw " << reg << "," << var.offset << "(s0)" << std::endl;
                stream << "addi " << reg << "," << reg << ",1" << std::endl;
                stream << "sw " << reg << "," << var.offset << "(s0)" << std::endl;
                context.FreeTemporary(reg);
                // Store the incremented value in the destination register
                unaryChild_->EmitRISC(stream, context, destReg);
                break;
            }
            case UnaryOperator::PrefixDecrement: {
                Variable var = context.CurrentFrame().bindings.at(unaryChild_->GetIdentifier());
                Register reg = context.AllocateTemporary();
                stream << "lw " << reg << "," << var.offset << "(sp)" << std::endl;
                stream << "addi " << reg << "," << reg << ",-1" << std::endl;
                stream << "sw " << reg << "," << var.offset << "(sp)" << std::endl;
                context.FreeTemporary(reg);
                // Store the decremented value in the destination register
                unaryChild_->EmitRISC(stream, context, destReg);
                break;
            }
                // Below all multiplicative child expression
            case UnaryOperator::AddressOf: {
                // Variable is of non-pointer type - get offset + frame pointer
                Variable atAddress = context.CurrentFrame().bindings.at(multiplicativeChild_->GetIdentifier());
                stream << "addi " << destReg << ",sp," << atAddress.offset << std::endl;
                break;
            }
            case UnaryOperator::Dereference: {
                // Variable is a pointer - get value at address in variable
                // todo maybe abstract some of the bindings searches so we don't get segfaults
                Variable ptr = context.CurrentFrame().bindings.at(multiplicativeChild_->GetIdentifier());
                // Reuse destReg rather than waste a temporary
                stream << "lw " << destReg << "," << ptr.offset << "(sp)" << std::endl;
                stream << "lw " << destReg << ",0(" << destReg << ")" << std::endl;
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
        }
    }

    // We don't use references since at a lower level there may be a cast leaving a dangling reference
    std::string UnaryExpression::GetIdentifier() const {
        switch (op_) {
            case UnaryOperator::PostfixPromote:
                return postfixChild_->GetIdentifier();
            case UnaryOperator::PrefixIncrement:
            case UnaryOperator::PrefixDecrement:
                return unaryChild_->GetIdentifier();
            case UnaryOperator::AddressOf:
            case UnaryOperator::Dereference: // This can be called on lhs or rhs of assignment, should work the same in both
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
                return TypeSpecifier::INT; // todo ptrs, maybe table required for this
            case UnaryOperator::Plus:
            case UnaryOperator::Minus:
            case UnaryOperator::BitwiseNot:
            case UnaryOperator::LogicalNot:
                return multiplicativeChild_->GetType(context);
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
        }
        std::cerr << "Invalid unary operator" << std::endl;
        exit(1);
    }

} // namespace ast
