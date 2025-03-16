#include "ast_multiplicative_unary_expressions.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    // Lvalue asserts are in GetIdentifier impls
    void UnaryExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        bool hasDestination = destReg != Register::zero;
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
                    Register tempReg2 = hasDestination ? destReg : context.AllocateTemporary();
                    stream << "lui " << tempReg2 << ",%hi(" << identifier << ")" << std::endl;
                    stream << "lw " << tempReg2 << ",%lo(" << identifier << ")(" << tempReg2 << ")" << std::endl;
                    stream << "addi " << tempReg << "," << tempReg2 << ","
                           << (op_ == UnaryOperator::PrefixIncrement ? "1" : "-1") << std::endl;
                    stream << "lui " << tempReg2 << ",%hi(" << identifier << ")" << std::endl;
                    stream << "sw " << tempReg << ",%lo(" << identifier << ")(" << tempReg2 << ")" << std::endl;
                    context.FreeTemporary(tempReg);
                    if (!hasDestination)
                        context.FreeTemporary(tempReg2);
                    // Store the incremented/decremented value in the destination register
                    if (destReg != Register::zero)
                        unaryChild_->EmitRISC(stream, context, destReg);
                } else {
                    Variable var = context.CurrentFrame().bindings.Get(identifier);
                    Register tempReg = hasDestination ? destReg : context.AllocateTemporary();
                    stream << "lw " << tempReg << "," << var.offset << "(s0)" << std::endl;
                    stream << "addi " << tempReg << "," << tempReg << ","
                           << (op_ == UnaryOperator::PrefixIncrement ? "1" : "-1") << std::endl;
                    stream << "sw " << tempReg << "," << var.offset << "(s0)" << std::endl;
                    if (!hasDestination)
                        context.FreeTemporary(tempReg);
                    // Store the incremented/decremented value in the destination register
                    if (destReg != Register::zero)
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
                    TypeSpecifier type = context.GetGlobalType(identifier).GetPointeeType();
                    switch (type) {
                        case TypeSpecifier::Type::INT:
                        case TypeSpecifier::Type::UNSIGNED:
                        case TypeSpecifier::Type::ENUM:
                        case TypeSpecifier::Type::POINTER:
                            stream << "lui " << destReg << ",%hi(" << identifier << ")" << std::endl;
                            stream << "lw " << destReg << ",%lo(" << identifier << ")(" << destReg << ")" << std::endl;
                            stream << "lw " << destReg << ",0(" << destReg << ")" << std::endl;
                            break;
                        case TypeSpecifier::Type::CHAR:
                            stream << "lui " << destReg << ",%hi(" << identifier << ")" << std::endl;
                            stream << "lw " << destReg << ",%lo(" << identifier << ")(" << destReg << ")" << std::endl;
                            stream << "lbu " << destReg << ",0(" << destReg << ")" << std::endl;
                            break;
                        case TypeSpecifier::Type::FLOAT:
                        case TypeSpecifier::Type::DOUBLE: {
                            Register tempReg = context.AllocateTemporary();
                            stream << "lui " << tempReg << ",%hi(" << identifier << ")" << std::endl;
                            stream << "lw " << tempReg << ",%lo(" << identifier << ")(" << tempReg << ")" << std::endl;
                            stream << (type == TypeSpecifier::Type::FLOAT ? "flw " : "fld ") << destReg << ",0(" << tempReg
                                   << ")" << std::endl;
                            context.FreeTemporary(tempReg);
                            break;
                        }
                        case TypeSpecifier::Type::VOID:
                        case TypeSpecifier::Type::STRUCT:
                        case TypeSpecifier::Type::ARRAY:
                            throw std::runtime_error("Unsupported type for dereference");
                    }
                } else {
                    // Variable is a pointer - get value at address in variable
                    // This should only be called for RHS
                    Variable ptr = context.CurrentFrame().bindings.Get(identifier);
                    TypeSpecifier type = ptr.type.GetPointeeType();
                    switch (type) {
                        case TypeSpecifier::Type::INT:
                        case TypeSpecifier::Type::UNSIGNED:
                        case TypeSpecifier::Type::ENUM:
                        case TypeSpecifier::Type::POINTER:
                            stream << "lw " << destReg << "," << ptr.offset << "(s0)" << std::endl;
                            stream << "lw " << destReg << ",0(" << destReg << ")" << std::endl;
                            break;
                        case TypeSpecifier::Type::CHAR:
                            stream << "lw " << destReg << "," << ptr.offset << "(s0)" << std::endl;
                            stream << "lbu " << destReg << ",0(" << destReg << ")" << std::endl;
                            break;
                        case TypeSpecifier::Type::FLOAT:
                        case TypeSpecifier::Type::DOUBLE: {
                            Register tempReg = context.AllocateTemporary();
                            stream << "lw " << tempReg << "," << ptr.offset << "(s0)" << std::endl;
                            stream << (type == TypeSpecifier::Type::FLOAT ? "flw " : "fld ") << destReg << ",0(" << tempReg
                                   << ")" << std::endl;
                            context.FreeTemporary(tempReg);
                            break;
                        }
                        case TypeSpecifier::Type::VOID:
                        case TypeSpecifier::Type::STRUCT:
                        case TypeSpecifier::Type::ARRAY:
                            throw std::runtime_error("Unsupported type for dereference");
                    }
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
                stream << "li " << destReg << "," << unaryChild_->GetType(context).GetTypeSize() << std::endl;
                break;
            case UnaryOperator::SizeofType:
                stream << "li " << destReg << "," << typeNameChild_->GetType(context).GetTypeSize() << std::endl;
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
                return {TypeSpecifier::POINTER, multiplicativeChild_->GetType(context)};
            case UnaryOperator::Dereference:
                return multiplicativeChild_->GetType(context);
            case UnaryOperator::Plus:
            case UnaryOperator::Minus:
            case UnaryOperator::BitwiseNot:
            case UnaryOperator::LogicalNot:
                return multiplicativeChild_->GetType(context);
            case UnaryOperator::SizeofUnary:
            case UnaryOperator::SizeofType:
                return TypeSpecifier::UNSIGNED;
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

    const Expression &UnaryExpression::GetArrayIndexExpression() const {
        return postfixChild_->GetArrayIndexExpression();
    }

    bool UnaryExpression::IsPointerDereference() const {
        return op_ == UnaryOperator::Dereference;
    }

    int UnaryExpression::Evaluate(Context &context) const {
        // Probably overkill
        Context dummy;
        switch (op_) {
            case UnaryOperator::PostfixPromote:
                return postfixChild_->Evaluate(context);
            case UnaryOperator::Plus:
                return multiplicativeChild_->Evaluate(context);
            case UnaryOperator::Minus:
                return -multiplicativeChild_->Evaluate(context);
            case UnaryOperator::BitwiseNot:
                return ~multiplicativeChild_->Evaluate(context);
            case UnaryOperator::LogicalNot:
                return !multiplicativeChild_->Evaluate(context);
            // Accept errors if context is rqd
            case UnaryOperator::SizeofUnary:
                return unaryChild_->GetType(dummy).GetTypeSize();
            case UnaryOperator::SizeofType:
                return typeNameChild_->GetType(dummy).GetTypeSize();
            case UnaryOperator::AddressOf:
            case UnaryOperator::Dereference:
            case UnaryOperator::PrefixDecrement:
            case UnaryOperator::PrefixIncrement:
                break;
        }
        throw std::runtime_error("UnaryExpression::Evaluate() called on a non-constant");
    }

    UnaryExpression::~UnaryExpression() = default;

} // namespace ast
