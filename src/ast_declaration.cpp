#include "ast_declaration.hpp"

namespace ast {

void Declaration::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
    for (const auto &initDeclarator: *initDeclaratorList_) {
        if (initDeclarator->IsFunction()) {
        } else {
            int size = 4;
            std::string identifier = initDeclarator->GetIdentifier();
            destReg = context.AllocateTemporary();
            context.CurrentFrame().size += size;
            stream << "addi sp, sp, -" << size << std::endl;
            if (initDeclarator->HasInitializer()) {
                switch (typeSpecifier_) {
                    case TypeSpecifier::INT:
                        initDeclarator->EmitRISC(stream, context, destReg);
                        context.CurrentFrame().bindings.insert_or_assign(identifier, Variable{
                            .offset = context.CurrentFrame().size,
                            .size = size,
                            .reg = destReg
                        });
                        stream << "sw " << destReg << "," << context.CurrentFrame().size << "(s0)" << std::endl;
                        break;

                    case TypeSpecifier::FLOAT:
                        initDeclarator->EmitRISC(stream, context, destReg);
                        context.CurrentFrame().bindings.insert_or_assign(identifier, Variable{
                            .offset = context.CurrentFrame().size,
                            .size = size,
                            .reg = destReg
                        });
                        stream << "sw " << destReg << "," << context.CurrentFrame().size << "(s0)" << std::endl;
                        break;

                    case TypeSpecifier::DOUBLE:
                        initDeclarator->EmitRISC(stream, context, destReg);
                        context.CurrentFrame().bindings.insert_or_assign(identifier, Variable{
                            .offset = context.CurrentFrame().size,
                            .size = size,
                            .reg = destReg
                        });
                        stream << "sw " << destReg << "," << context.CurrentFrame().size << "(s0)" << std::endl;
                        break;
                }
            } else {
                context.CurrentFrame().bindings.insert_or_assign(identifier, Variable{
                    .offset = context.CurrentFrame().size,
                    .size = size,
                    .reg = Register::zero
                });
            }
        }
    }
}

void Declaration::Print(std::ostream &stream) const {
    stream << typeSpecifier_;
    initDeclaratorList_->Print(stream);
}

}
