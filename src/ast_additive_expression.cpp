#include "ast_type_specifier.hpp"
#include "ast_additive_expression.hpp"

namespace ast {

    // todo nice to have : extract a binaryexpression class to handle common logic
    void AdditiveExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (op_ == AdditiveOperator::MultiplicativePromote) {
            right_->EmitRISC(stream, context, destReg);
            return;
        }

        TypeSpecifier type = GetType(context);
        bool leftStored = right_->ContainsFunctionCall();
        bool useFloat = type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE;
        Register leftReg = leftStored ? context.AllocatePersistent(useFloat) : context.AllocateTemporary(useFloat);
        left_->EmitRISC(stream, context, leftReg);
        Register rightReg = context.AllocateTemporary(useFloat);
        right_->EmitRISC(stream, context, rightReg);
        switch (type) {
            case TypeSpecifier::FLOAT:
                stream <<
                       (op_ == AdditiveOperator::Add ? "fadd.s " : "fsub.s ")
                       << destReg << "," << leftReg << "," << rightReg << std::endl;
                break;
            case TypeSpecifier::DOUBLE:
                stream <<
                       (op_ == AdditiveOperator::Add ? "fadd.d " : "fsub.d ")
                       << destReg << "," << leftReg << "," << rightReg << std::endl;
                break;
            case TypeSpecifier::POINTER: // todo extract type here for pointer arith... need both sides right? but check in godbolt
            case TypeSpecifier::INT:
            case TypeSpecifier::UNSIGNED:
                stream <<
                       (op_ == AdditiveOperator::Add ? "add " : "sub ")
                       << destReg << "," << leftReg << "," << rightReg << std::endl;
                break;
            case TypeSpecifier::CHAR:
                stream <<
                       (op_ == AdditiveOperator::Add ? "add " : "sub ")
                       << destReg << "," << leftReg << "," << rightReg << std::endl;
                break;
            case TypeSpecifier::Type::ENUM:
            case TypeSpecifier::Type::ARRAY:
            case TypeSpecifier::Type::STRUCT:
            case TypeSpecifier::Type::VOID:
                throw std::runtime_error("Addition on that type isn't supported yet!");
                // TODO it should be supported - array as ptr, enum as underlying void and struct are actually unsupported
        }
        leftStored ? context.FreePersistent(leftReg) : context.FreeTemporary(leftReg);
        context.FreeTemporary(rightReg);
    }

    void AdditiveExpression::Print(std::ostream &stream) const {
        if (left_ != nullptr) {
            left_->Print(stream);
        }

        switch (op_) {
            case AdditiveOperator::Add:
                stream << " + ";
                break;
            case AdditiveOperator::Subtract:
                stream << " - ";
                break;
            case AdditiveOperator::MultiplicativePromote:
                break;
        }

        right_->Print(stream);
    }


    TypeSpecifier AdditiveExpression::GetType(Context &context) const {
        // todo technically should be l or r e.g. for float, need some ordering/ranking of types and propagate elsewhere
        return right_->GetType(context);
    }

    bool AdditiveExpression::ContainsFunctionCall() const {
        if (op_ == AdditiveOperator::MultiplicativePromote) {
            return right_->ContainsFunctionCall();
        } else {
            return (left_ != nullptr && left_->ContainsFunctionCall()) || right_->ContainsFunctionCall();
        }
    }

    std::string AdditiveExpression::GetGlobalIdentifier() const {
        return right_->GetGlobalIdentifier();
    }

    int AdditiveExpression::GetGlobalValue() const {
        return right_->GetGlobalValue();
    }

} // namespace ast
