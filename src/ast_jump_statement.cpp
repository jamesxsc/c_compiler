#include <cassert>
#include "ast_jump_statement.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void ReturnStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        assert(context.CurrentFrame().returnLabel.has_value() && "Return statement outside of a function");

        TypeSpecifier returnType = context.CurrentFrame().returnType;
        if (returnType == TypeSpecifier::Type::VOID || expression_ == nullptr) {
            stream << "j " << *context.CurrentFrame().returnLabel << std::endl;
            return;
        }
        switch (returnType) {
            case TypeSpecifier::Type::INT:
            case TypeSpecifier::Type::CHAR:
            case TypeSpecifier::Type::UNSIGNED:
            case TypeSpecifier::Type::POINTER:
            case TypeSpecifier::Type::ENUM:
                expression_->EmitRISC(stream, context, Register::a0);
                break;
            case TypeSpecifier::Type::FLOAT:
            case TypeSpecifier::Type::DOUBLE:
                expression_->EmitRISC(stream, context, Register::fa0);
                break;
            case TypeSpecifier::Type::STRUCT: {
                // Handled in Identifier, better not be something stupid
                expression_->EmitRISC(stream, context, Register::a0);
                break;
            }
            case TypeSpecifier::Type::ARRAY: // Can't return arrays
            case TypeSpecifier::Type::VOID: // Void handled by nullptr check
                throw std::runtime_error("ReturnStatement::EmitRISC() called on an unsupported type");
        }

        stream << "j " << *context.CurrentFrame().returnLabel << std::endl;
    }

    void ReturnStatement::Print(std::ostream &stream) const {
        stream << "return";
        if (expression_ != nullptr) {
            stream << " ";
            expression_->Print(stream);
        }
        stream << ";" << std::endl;
    }


    TypeSpecifier ReturnStatement::GetType(Context &context) const {
        return context.CurrentFrame().returnType;
    }

    void BreakStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        assert(context.CurrentFrame().breakLabel.size() > 0 && "Break statement outside of loop or switch");
        stream << "j " << context.CurrentFrame().breakLabel.back() << std::endl;
    }

    void BreakStatement::Print(std::ostream &stream) const {
        stream << "break;" << std::endl;
    }

    void ContinueStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        assert(context.CurrentFrame().continueLabel.size() > 0 && "Continue statement outside of loop");
        stream << "j " << context.CurrentFrame().continueLabel.back() << std::endl;
    }

    void ContinueStatement::Print(std::ostream &stream) const {
        stream << "continue;" << std::endl;
    }
}
