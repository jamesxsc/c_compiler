#include "ast_argument_expression_list.hpp"

namespace ast {

    void ArgumentExpressionList::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (arguments_.size() > 8) {
            std::cerr << "More than 8 arguments in a function call is unsupported" << std::endl;
            // todo support this and equiv in parameter list
            exit(1);
        } else {
            Register firstIntegerReg = Register::a0;
            Register firstFloatReg = Register::fa0;
            for (const auto &argument: arguments_) {
                TypeSpecifier type = argument->GetType(context);
                if (type == TypeSpecifier::STRUCT) {
                    if (type.UseStack()) {
                        // It will be copied if necessary by the callee, so we just return the address
                        bool restore = context.SetEmitLHS(true);
                        argument->EmitRISC(stream, context, firstIntegerReg);
                        context.SetEmitLHS(restore);
                        firstIntegerReg = static_cast<Register>(static_cast<int>(firstIntegerReg) + 1);
                    } else {
                        // Load all of the members into registers
                        int memberOffset = 0;
                        // Get the base address
                        Register baseAddressReg = context.AllocateTemporary();
                        bool restore = context.SetEmitLHS(true);
                        argument->EmitRISC(stream, context, baseAddressReg);
                        context.SetEmitLHS(restore);
                        for (const auto &member: type.GetStructMembers()) {
                            if (member.first == "#padding") {
                                memberOffset += member.second.GetTypeSize();
                                continue;
                            }

                            switch (member.second) {
                                case TypeSpecifier::INT:
                                case TypeSpecifier::POINTER:
                                case TypeSpecifier::UNSIGNED:
                                case TypeSpecifier::ENUM:
                                case TypeSpecifier::ARRAY: // todo is this right? cx everything arrays in structs
                                    stream << "lw " << firstIntegerReg << "," << memberOffset << "(" << baseAddressReg
                                           << ")" << std::endl;
                                    firstIntegerReg = static_cast<Register>(static_cast<int>(firstIntegerReg) + 1);
                                    break;
                                case TypeSpecifier::CHAR:
                                    stream << "lbu " << firstIntegerReg << "," << memberOffset << "(" << baseAddressReg
                                           << ")" << std::endl;
                                    firstIntegerReg = static_cast<Register>(static_cast<int>(firstIntegerReg) + 1);
                                    break;
                                case TypeSpecifier::FLOAT:
                                case TypeSpecifier::DOUBLE:
                                    stream << (member.second == TypeSpecifier::FLOAT ? "flw " : "fld ") << firstFloatReg
                                           << "," << memberOffset << "(" << baseAddressReg << ")" << std::endl;
                                    firstFloatReg = static_cast<Register>(static_cast<int>(firstFloatReg) + 1);
                                    break;
                                case TypeSpecifier::STRUCT: // todo Handle nested structs
                                case TypeSpecifier::VOID:
                                    std::cerr
                                            << "ArgumentExpressionList::EmitRISC() called on an unsupported struct member type"
                                            << std::endl;
                                    exit(1);
                            }
                            memberOffset += member.second.GetTypeSize();
                        }
                        context.FreeTemporary(baseAddressReg);
                    }
                } else if (type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE) {
                    argument->EmitRISC(stream, context, firstFloatReg);
                    firstFloatReg = static_cast<Register>(static_cast<int>(firstFloatReg) + 1);
                } else {
                    argument->EmitRISC(stream, context, firstIntegerReg);
                    firstIntegerReg = static_cast<Register>(static_cast<int>(firstIntegerReg) + 1);
                }
            }
        }
    }

    void ArgumentExpressionList::Print(std::ostream &stream) const {
        for (auto it = arguments_.begin(); it != arguments_.end(); ++it) {
            (*it)->Print(stream);
            if (it + 1 != arguments_.end()) {
                stream << ", ";
            }
        }
    }

    void ArgumentExpressionList::PushBack(AssignmentExpressionPtr item) {
        arguments_.push_back(std::move(item));
    }

    size_t ArgumentExpressionList::Size() const {
        return arguments_.size();
    }

    std::vector<AssignmentExpressionPtr>::const_iterator ArgumentExpressionList::begin() const {
        return arguments_.begin();
    }

    std::vector<AssignmentExpressionPtr>::const_iterator ArgumentExpressionList::end() const {
        return arguments_.end();
    }

}
