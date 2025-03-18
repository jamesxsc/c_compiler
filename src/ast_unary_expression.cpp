#include <cassert>
#include "ast_unary_expression.hpp"
#include "ast_multiplicative_expression.hpp"
#include "ast_type_specifier.hpp"
#include "risc_utils.hpp"

namespace ast {

    // Lvalue asserts are in GetIdentifier impls
    void UnaryExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (context.EmitLHS() && op_ == UnaryOperator::Dereference) {
            context.dereference = true;
            multiplicativeChild_->EmitRISC(stream, context, destReg);
            context.dereference = false;
            return;
        }

        switch (op_) {
            case UnaryOperator::PostfixPromote:
                postfixChild_->EmitRISC(stream, context, destReg);
                break;
            case UnaryOperator::PrefixIncrement:
            case UnaryOperator::PrefixDecrement: {
                Utils::EmitIncrementDecrement(stream, context, destReg, *unaryChild_,
                                              op_ == UnaryOperator::PrefixDecrement, false);
                break;
            }
                // Below all multiplicative child expression
            case UnaryOperator::AddressOf: {
                // This will emit the raw address
                bool restore = context.SetEmitLHS(true);
                multiplicativeChild_->EmitRISC(stream, context, destReg);
                context.SetEmitLHS(restore);
                break;
            }
            case UnaryOperator::Dereference: {
                context.dereference = true;
                TypeSpecifier pointeeType = multiplicativeChild_->GetType(context).GetPointeeType();
                switch (pointeeType) {
                    case TypeSpecifier::Type::INT:
                    case TypeSpecifier::Type::UNSIGNED:
                    case TypeSpecifier::Type::POINTER:
                    case TypeSpecifier::Type::ENUM:
                        multiplicativeChild_->EmitRISC(stream, context, destReg);
                        stream << "lw " << destReg << ",0(" << destReg << ")" << std::endl;
                        break;
                    case TypeSpecifier::Type::CHAR:
                        multiplicativeChild_->EmitRISC(stream, context, destReg);
                        stream << "lbu " << destReg << ",0(" << destReg << ")" << std::endl;
                        break;
                    case TypeSpecifier::Type::DOUBLE:
                    case TypeSpecifier::Type::FLOAT: {
                        Register addressReg = context.AllocateTemporary();
                        multiplicativeChild_->EmitRISC(stream, context, addressReg);
                        stream << (pointeeType == TypeSpecifier::Type::FLOAT ? "flw " : "fld ") << destReg << ",0("
                               << addressReg << ")" << std::endl;
                        context.FreeTemporary(addressReg);
                        break;
                    }
                    case TypeSpecifier::Type::STRUCT:
                    case TypeSpecifier::Type::ARRAY:
                    case TypeSpecifier::Type::VOID:
                        throw std::runtime_error("Unsupported type for dereference right now, sorry");
                }
                context.dereference = false;
                break;
            }
            case UnaryOperator::Plus:
                // Does nothing
                multiplicativeChild_->EmitRISC(stream, context, destReg);
                break;
            case UnaryOperator::Minus: {
                multiplicativeChild_->EmitRISC(stream, context, destReg);
                TypeSpecifier type = multiplicativeChild_->GetType(context);
                switch (type) {
                    case TypeSpecifier::Type::INT:
                    case TypeSpecifier::Type::CHAR:
                    case TypeSpecifier::Type::UNSIGNED:
                    case TypeSpecifier::Type::ENUM:
                        stream << "neg " << destReg << "," << destReg << std::endl;
                        break;
                    case TypeSpecifier::Type::FLOAT:
                        stream << "fneg.s " << destReg << "," << destReg << std::endl;
                        break;
                    case TypeSpecifier::Type::DOUBLE:
                        stream << "fneg.d " << destReg << "," << destReg << std::endl;
                        break;
                    case TypeSpecifier::Type::VOID:
                    case TypeSpecifier::Type::POINTER:
                    case TypeSpecifier::Type::STRUCT:
                    case TypeSpecifier::Type::ARRAY:
                        throw std::runtime_error("Unary minus attempted on unsupported type");
                }
                break;
            }
            case UnaryOperator::BitwiseNot:
                multiplicativeChild_->EmitRISC(stream, context, destReg);
                stream << "not " << destReg << "," << destReg << std::endl;
                break;
            case UnaryOperator::LogicalNot:
                Utils::EmitComparison(stream, context, destReg, *multiplicativeChild_);
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
                if (context.EmitLHS())
                    return multiplicativeChild_->GetType(context).GetPointeeType();
                else
                    return multiplicativeChild_->GetType(context);
            case UnaryOperator::Plus:
            case UnaryOperator::Minus:
            case UnaryOperator::BitwiseNot:
            case UnaryOperator::LogicalNot:
                return TypeSpecifier::INT;
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

    double UnaryExpression::EvaluateFloat(ast::Context &context) const {
        // Probably overkill
        Context dummy;
        switch (op_) {
            case UnaryOperator::PostfixPromote:
                return postfixChild_->EvaluateFloat(context);
            case UnaryOperator::Plus:
                return multiplicativeChild_->EvaluateFloat(context);
            case UnaryOperator::Minus:
                return -multiplicativeChild_->EvaluateFloat(context);
                // Accept errors if context is rqd
            case UnaryOperator::SizeofUnary:
                return unaryChild_->GetType(dummy).GetTypeSize();
            case UnaryOperator::SizeofType:
                return typeNameChild_->GetType(dummy).GetTypeSize();
            case UnaryOperator::Dereference:
            case UnaryOperator::AddressOf:
            case UnaryOperator::BitwiseNot:
            case UnaryOperator::LogicalNot:
            case UnaryOperator::PrefixDecrement:
            case UnaryOperator::PrefixIncrement:
                break;
        }
        throw std::runtime_error("UnaryExpression::Evaluate() called on a non-constant");
    }

    UnaryExpression::UnaryExpression(PostfixExpressionPtr child) : postfixChild_(std::move(child)),
                                                                   unaryChild_(nullptr),
                                                                   multiplicativeChild_(nullptr),
                                                                   typeNameChild_(nullptr),
                                                                   op_(UnaryOperator::PostfixPromote) {}

    UnaryExpression::UnaryExpression(TypeNamePtr child) : postfixChild_(nullptr),
                                                          unaryChild_(nullptr),
                                                          multiplicativeChild_(nullptr),
                                                          typeNameChild_(std::move(child)),
                                                          op_(UnaryOperator::SizeofType) {}

    UnaryExpression::UnaryExpression(UnaryExpressionPtr child, UnaryOperator op) : postfixChild_(nullptr),
                                                                                   unaryChild_(std::move(child)),
                                                                                   multiplicativeChild_(nullptr),
                                                                                   typeNameChild_(nullptr),
                                                                                   op_(op) {}

    UnaryExpression::UnaryExpression(MultiplicativeExpressionPtr child, UnaryOperator op)
            : multiplicativeChild_(std::move(child)), op_(op) {}


    UnaryExpression::~UnaryExpression() = default;

} // namespace ast
