#include "ast_equality_expression.hpp"
#include "ast_type_specifier.hpp"
#include "risc_utils.hpp"

namespace ast {

    void EqualityExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (op_ == EqualityOperator::RelationalPromote) {
            right_->EmitRISC(stream, context, destReg);
            return;
        }

        TypeSpecifier type = Utils::BinaryResultType(left_->GetType(context), right_->GetType(context));
        bool useFloat = type == TypeSpecifier::Type::FLOAT || type == TypeSpecifier::Type::DOUBLE;
        bool leftStored = right_->ContainsFunctionCall();
        Register leftReg = leftStored ? context.AllocatePersistent(useFloat) : context.AllocateTemporary(useFloat);
        left_->EmitRISC(stream, context, leftReg);
        Register rightReg = context.AllocateTemporary(useFloat);
        right_->EmitRISC(stream, context, rightReg);
        switch (op_) {
            case EqualityOperator::Equality:
                switch (type) {
                    case TypeSpecifier::Type::INT:
                    case TypeSpecifier::Type::ENUM:
                    case TypeSpecifier::Type::CHAR:
                    case TypeSpecifier::Type::UNSIGNED:
                    case TypeSpecifier::Type::POINTER:
                    case TypeSpecifier::Type::ARRAY:
                        stream << "sub " << destReg << "," << leftReg << "," << rightReg << std::endl;
                        stream << "seqz " << destReg << "," << destReg << std::endl;
                        break;
                    case TypeSpecifier::Type::FLOAT:
                    case TypeSpecifier::Type::DOUBLE:
                        stream << (type == TypeSpecifier::Type::FLOAT ? "feq.s " : "feq.d ") << destReg << "," << leftReg << "," << rightReg << std::endl;
                        break;
                    case TypeSpecifier::Type::STRUCT:
                    case TypeSpecifier::Type::VOID:
                        throw std::runtime_error("EqualityExpression::EmitRISC() called on an unsupported type");
                }
                stream << "sub " << destReg << "," << leftReg << "," << rightReg << std::endl;
                stream << "seqz " << destReg << "," << destReg << std::endl;
                break;
            case EqualityOperator::Inequality:
                switch (type) {
                    case TypeSpecifier::Type::INT:
                    case TypeSpecifier::Type::ENUM:
                    case TypeSpecifier::Type::CHAR:
                    case TypeSpecifier::Type::UNSIGNED:
                    case TypeSpecifier::Type::POINTER:
                    case TypeSpecifier::Type::ARRAY:
                        stream << "sub " << destReg << "," << leftReg << "," << rightReg << std::endl;
                        stream << "snez " << destReg << "," << destReg << std::endl;
                        break;
                    case TypeSpecifier::Type::FLOAT:
                    case TypeSpecifier::Type::DOUBLE:
                        stream << (type == TypeSpecifier::Type::FLOAT ? "feq.s " : "feq.d ") << destReg << "," << leftReg << "," << rightReg << std::endl;
                        stream << "seqz " << destReg << "," << destReg << std::endl;
                        break;
                    case TypeSpecifier::Type::STRUCT:
                    case TypeSpecifier::Type::VOID:
                        throw std::runtime_error("EqualityExpression::EmitRISC() called on an unsupported type");
                }
                break;
            case EqualityOperator::RelationalPromote: // Should never reach here
                break;
        }
        stream << "andi " << destReg << "," << destReg << ",0xff" << std::endl;
        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg);
        context.FreeTemporary(rightReg);
    }

    void EqualityExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr) {
            left_->Print(stream);
        }
        switch (op_) {
            case EqualityOperator::Equality: {
                stream << " == ";
                break;
            }
            case EqualityOperator::Inequality: {
                stream << " != ";
                break;
            }
            case EqualityOperator::RelationalPromote: {
                break;
            }
        }
        right_->Print(stream);
    }

    TypeSpecifier EqualityExpression::GetType(Context & context) const {
        if (op_ == EqualityOperator::RelationalPromote) {
            return right_->GetType(context);
        }

        return TypeSpecifier::INT;
    }

    bool EqualityExpression::ContainsFunctionCall() const {
        if (op_ == EqualityOperator::RelationalPromote) {
            return right_->ContainsFunctionCall();
        } else {
            return left_->ContainsFunctionCall() || right_->ContainsFunctionCall();
        }
    }

    // These are for constants which will always simply be a RelationalPromote
    std::string EqualityExpression::GetGlobalIdentifier() const {
        return right_->GetGlobalIdentifier();
    }

    int EqualityExpression::GetGlobalValue() const {
        return right_->GetGlobalValue();
    }

    int EqualityExpression::Evaluate() const {
        switch (op_) {
            case EqualityOperator::RelationalPromote:
                return right_->Evaluate();
            case EqualityOperator::Equality:
                return left_->Evaluate() == right_->Evaluate();
            case EqualityOperator::Inequality:
                return left_->Evaluate() != right_->Evaluate();
        }
        throw std::runtime_error("EqualityExpression::Evaluate() reached end of function");
    }

}