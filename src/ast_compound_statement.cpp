#include "ast_compound_statement.hpp"

namespace ast {

    void CompoundStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (declarations_)
            declarations_->EmitRISC(stream, context, destReg);
        if (statements_)
            statements_->EmitRISC(stream, context, destReg);
    }

    void CompoundStatement::Print(std::ostream &stream) const {
        if (declarations_)
            declarations_->Print(stream);
        if (statements_)
            statements_->Print(stream);
    }

}
