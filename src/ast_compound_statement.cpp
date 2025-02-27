#include "ast_compound_statement.hpp"

namespace ast {

    void CompoundStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (!isFunction_) { // Function scope is handled by declarator
            context.PushScope();
        }

        if (declarations_)
            declarations_->EmitRISC(stream, context, destReg);
        if (statements_)
            statements_->EmitRISC(stream, context, destReg);

        if (!isFunction_) {
            context.PopScope(stream);
        }
    }

    void CompoundStatement::Print(std::ostream &stream) const {
        stream << "{\n";
        if (declarations_)
            declarations_->Print(stream);
        if (statements_)
            statements_->Print(stream);
        stream << "}\n";
    }

    bool CompoundStatement::IsFunction() const {
        return isFunction_;
    }

    void CompoundStatement::SetFunction() {
        isFunction_ = true;
    }

}
