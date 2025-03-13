#include <cassert>
#include "ast_jump_statement.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    void ReturnStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        assert(context.CurrentFrame().returnLabel.has_value() && "Return statement outside of a function");

        Register returnReg =
                (GetType(context) == TypeSpecifier::FLOAT ||
                 GetType(context) == TypeSpecifier::DOUBLE)
                ? Register::fa0 : Register::a0;
        // this may become a ballache if (when) we have to return a struct

        if (expression_ != nullptr) {
            expression_->EmitRISC(stream, context, returnReg);
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
        if (expression_)
            return expression_->GetType(context);
        else
            return TypeSpecifier::INT; // todo fuck that
    }

    void BreakStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        assert(context.CurrentFrame().breakLabel && "Break statement outside of loop or switch");
        stream << "j " << *context.CurrentFrame().breakLabel << std::endl;
    }

    void BreakStatement::Print(std::ostream &stream) const {
        stream << "break;" << std::endl;
    }

    void ContinueStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        assert(context.CurrentFrame().continueLabel && "Continue statement outside of loop");
        stream << "j " << *context.CurrentFrame().continueLabel << std::endl;
    }

    void ContinueStatement::Print(std::ostream &stream) const {
        stream << "continue;" << std::endl;
    }
}
