#include "ast_relational_expression.hpp"
#include "ast_type_specifier.hpp"
#include "risc_utils.hpp"

namespace ast {

    void RelationalExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (op_ == RelationalOperator::ShiftPromote) {
            right_->EmitRISC(stream, context, destReg);
            return;
        }

        TypeSpecifier type = Utils::BinaryResultType(left_->GetType(context), right_->GetType(context));
        bool leftStored = right_->ContainsFunctionCall();
        bool useFloat = type == TypeSpecifier::Type::FLOAT || type == TypeSpecifier::Type::DOUBLE;
        Register leftReg = leftStored ? context.AllocatePersistent(useFloat) : context.AllocateTemporary(useFloat);
        left_->EmitRISC(stream, context, leftReg);
        Register rightReg = context.AllocateTemporary(useFloat);
        right_->EmitRISC(stream, context, rightReg);
        switch (type) {
            case TypeSpecifier::Type::INT:
            case TypeSpecifier::Type::ENUM:
                switch (op_) {
                    case RelationalOperator::LessThan:
                        stream << "slt " << destReg << "," << leftReg << "," << rightReg << std::endl;
                        break;
                    case RelationalOperator::GreaterThan:
                        stream << "sgt " << destReg << "," << leftReg << "," << rightReg << std::endl;
                        break;
                    case RelationalOperator::LessThanOrEqual:
                        stream << "sgt " << destReg << "," << leftReg << "," << rightReg << std::endl;
                        stream << "seqz " << destReg << "," << destReg << std::endl;
                        break;
                    case RelationalOperator::GreaterThanOrEqual:
                        stream << "slt " << destReg << "," << leftReg << "," << rightReg << std::endl;
                        stream << "seqz " << destReg << "," << destReg << std::endl;
                        break;
                    case RelationalOperator::ShiftPromote: // Should never reach here
                        break;
                }
                stream << "andi " << destReg << "," << destReg << ",0xff" << std::endl;
                break;
            case TypeSpecifier::Type::CHAR:
            case TypeSpecifier::Type::POINTER: // Treated as unsigned
            case TypeSpecifier::Type::ARRAY: // Appears to be treated as pointer
            case TypeSpecifier::Type::UNSIGNED:
                switch (op_) {
                    case RelationalOperator::LessThan:
                        stream << "sltu " << destReg << "," << leftReg << "," << rightReg << std::endl;
                        break;
                    case RelationalOperator::GreaterThan:
                        stream << "sgtu " << destReg << "," << leftReg << "," << rightReg << std::endl;
                        break;
                    case RelationalOperator::LessThanOrEqual:
                        stream << "sgtu " << destReg << "," << leftReg << "," << rightReg << std::endl;
                        stream << "seqz " << destReg << "," << destReg << std::endl;
                        break;
                    case RelationalOperator::GreaterThanOrEqual:
                        stream << "sltu " << destReg << "," << leftReg << "," << rightReg << std::endl;
                        stream << "seqz " << destReg << "," << destReg << std::endl;
                        break;
                    case RelationalOperator::ShiftPromote: // Should never reach here
                        break;
                }
                stream << "andi " << destReg << "," << destReg << ",0xff" << std::endl;
                break;
            case TypeSpecifier::Type::DOUBLE:
            case TypeSpecifier::Type::FLOAT:
                switch (op_) {
                    case RelationalOperator::LessThan:
                        stream << (type == TypeSpecifier::Type::FLOAT ? "flt.s " : "flt.d ")
                               << destReg << "," << leftReg << "," << rightReg << std::endl;
                        break;
                    case RelationalOperator::GreaterThan:
                        stream << (type == TypeSpecifier::Type::FLOAT ? "fgt.s " : "fgt.d ")
                                << destReg << "," << leftReg << "," << rightReg << std::endl;
                        break;
                    case RelationalOperator::LessThanOrEqual:
                        stream << (type == TypeSpecifier::Type::FLOAT ? "fle.s " : "fle.d ")
                               << destReg << "," << leftReg << "," << rightReg << std::endl;
                        break;
                    case RelationalOperator::GreaterThanOrEqual:
                        stream << (type == TypeSpecifier::Type::FLOAT ? "fge.s " : "fge.d ")
                               << destReg << "," << leftReg << "," << rightReg << std::endl;
                        break;
                    case RelationalOperator::ShiftPromote: // Should never reach here
                        break;
                }
                stream << "andi " << destReg << "," << destReg << ",0xff" << std::endl;
                break;
            case TypeSpecifier::Type::VOID:
            case TypeSpecifier::Type::STRUCT:
                throw std::runtime_error("RelationalExpression::EmitRISC() called on an unsupported type");
        }

        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg);
        context.FreeTemporary(rightReg);
    }

    void RelationalExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr)
            left_->Print(stream);
        switch (op_) {
            case RelationalOperator::LessThan:
                stream << " < ";
                break;
            case RelationalOperator::GreaterThan:
                stream << " > ";
                break;
            case RelationalOperator::LessThanOrEqual:
                stream << " <= ";
                break;
            case RelationalOperator::GreaterThanOrEqual:
                stream << " >= ";
                break;
            case RelationalOperator::ShiftPromote:
                break;
        }
        right_->Print(stream);
    }

    TypeSpecifier RelationalExpression::GetType(Context &context) const {
        if (op_ == RelationalOperator::ShiftPromote) {
            return right_->GetType(context);
        }

        return TypeSpecifier::INT;
    }

    bool RelationalExpression::ContainsFunctionCall() const {
        if (op_ == RelationalOperator::ShiftPromote) {
            return right_->ContainsFunctionCall();
        } else {
            return (left_ != nullptr && left_->ContainsFunctionCall()) || right_->ContainsFunctionCall();
        }
    }

    std::string RelationalExpression::GetGlobalIdentifier() const {
        return right_->GetGlobalIdentifier();
    }

    int RelationalExpression::GetGlobalValue() const {
        return right_->GetGlobalValue();
    }

}
