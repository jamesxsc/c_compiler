#include "ast_switch_statement.hpp"
#include "risc_utils.hpp"
#include <sstream>

namespace ast {

    void SwitchStatement::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (body_) {
            std::string endLabel = context.MakeLabel(".SWITCH_END");
            context.CurrentFrame().breakLabel.push_back(endLabel);

            std::stringstream bodyBuffer;
            body_->SetInSwitchScope();
            body_->EmitRISC(bodyBuffer, context, destReg);

            // Emit comparisons and jumps
            // This is actually more efficient than GCC with -O0
            // Do not put them in instance or nested switch will break
            Register condReg = context.AllocateTemporary();
            condition_->EmitRISC(stream, context, condReg);
            std::string defaultLabel{endLabel};
            for (auto &pair: body_->GetSwitchLabelCasePairs()) {
                if (!pair.second.has_value()) {
                    defaultLabel = pair.first;
                    continue;
                }
                std::string label = pair.first;
                Register constexprReg = context.AllocateTemporary();
                stream << "li " << constexprReg << "," << *pair.second << std::endl;
                stream << "beq " << condReg << "," << constexprReg << "," << label << std::endl;
                context.FreeRegister(constexprReg);
            }
            // Default or end
            stream << "j " << defaultLabel << std::endl;

            context.FreeRegister(condReg);

            if (bodyBuffer.rdbuf()->in_avail() > 0)
                stream << bodyBuffer.rdbuf();

            stream << endLabel << ":" << std::endl;

            context.CurrentFrame().breakLabel.pop_back();
        }

    }

    void SwitchStatement::Print(std::ostream &stream) const {
        stream << "switch (";
        condition_->Print(stream);
        stream << ")";

        if (body_) {
            body_->Print(stream);
        }
    }

} // namespace ast
