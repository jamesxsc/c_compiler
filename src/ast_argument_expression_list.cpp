#include "ast_argument_expression_list.hpp"

namespace ast {

    void ArgumentExpressionList::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        Register firstIntegerReg = Register::a0;
        Register firstFloatReg = Register::fa0;
        int stackOffset = 0;
        for (const auto &argument: arguments_) {
            TypeSpecifier type = argument->GetType(context);
            if (type == TypeSpecifier::STRUCT) {
                if (type.UseStack()) {
                    if (firstIntegerReg > Register::a7) { // Out of registers; use stack
                        Register tempReg = context.AllocateTemporary(stream);
                        bool restore = context.SetEmitLHS(true);
                        argument->EmitRISC(stream, context, tempReg);
                        context.SetEmitLHS(restore);
                        // Store pointer
                        stream << "sw " << tempReg << "," << stackOffset << "(sp)" << std::endl;
                        stackOffset += 4; // pointer size
                        context.FreeTemporary(tempReg, stream);
                    } else {
                        // It will be copied if necessary by the callee, so we just return the address
                        bool restore = context.SetEmitLHS(true);
                        argument->EmitRISC(stream, context, firstIntegerReg);
                        context.SetEmitLHS(restore);
                        firstIntegerReg = static_cast<Register>(static_cast<int>(firstIntegerReg) + 1);
                    }
                } else {
                    // Load all of the members into registers
                    int memberOffset = 0;
                    // Get the base address
                    Register baseAddressReg = context.AllocateTemporary(stream);
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
                            case TypeSpecifier::CHAR:
                            case TypeSpecifier::ARRAY: // todo is this right? cx everything arrays in structs
                                if (firstIntegerReg > Register::a7) {
                                    Register tempReg = context.AllocateTemporary(stream);
                                    stream << (type == TypeSpecifier::CHAR ? "lbu " : "lw ") << tempReg << ","
                                           << memberOffset << "(" << baseAddressReg << ")" << std::endl;
                                    stream << (type == TypeSpecifier::CHAR ? "sb " : "sw ") << tempReg << ","
                                           << stackOffset << "(sp)" << std::endl;
                                    stackOffset += member.second.GetTypeSize();
                                    context.FreeTemporary(tempReg, stream);
                                } else {
                                    stream << (type == TypeSpecifier::CHAR ? "lbu " : "lw ") << firstIntegerReg << ","
                                           << memberOffset << "(" << baseAddressReg << ")" << std::endl;
                                    firstIntegerReg = static_cast<Register>(static_cast<int>(firstIntegerReg) + 1);
                                }
                                break;
                            case TypeSpecifier::FLOAT:
                            case TypeSpecifier::DOUBLE:
                                if (firstFloatReg > Register::fa7) {
                                    Register tempReg = context.AllocateTemporary(stream, true);
                                    stream << (member.second == TypeSpecifier::FLOAT ? "flw " : "fld ") << tempReg
                                           << ","
                                           << memberOffset << "(" << baseAddressReg << ")" << std::endl;
                                    stream << (member.second == TypeSpecifier::FLOAT ? "fsw " : "fsd ") << tempReg
                                           << ","
                                           << stackOffset << "(sp)" << std::endl;
                                    stackOffset += member.second.GetTypeSize();
                                    context.FreeTemporary(tempReg, stream);
                                } else {
                                    stream << (member.second == TypeSpecifier::FLOAT ? "flw " : "fld ") << firstFloatReg
                                           << "," << memberOffset << "(" << baseAddressReg << ")" << std::endl;
                                    firstFloatReg = static_cast<Register>(static_cast<int>(firstFloatReg) + 1);
                                }
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
                    context.FreeTemporary(baseAddressReg, stream);
                }
            } else if (type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE) {
                if (firstFloatReg > Register::fa7) { // Out of registers; use stack
                    Register tempReg = context.AllocateTemporary(stream, true);
                    argument->EmitRISC(stream, context, tempReg);
                    stream << (type == TypeSpecifier::FLOAT ? "fsw " : "fsd ") << tempReg << "," << stackOffset
                           << "(sp)" << std::endl;
                    stackOffset += type.GetTypeSize();
                    context.FreeTemporary(tempReg, stream);
                } else {
                    argument->EmitRISC(stream, context, firstFloatReg);
                    firstFloatReg = static_cast<Register>(static_cast<int>(firstFloatReg) + 1);
                }
            } else {
                if (firstIntegerReg > Register::a7) { // Out of registers; use stack
                    Register tempReg = context.AllocateTemporary(stream);
                    argument->EmitRISC(stream, context, tempReg);
                    stream << "sw " << tempReg << "," << stackOffset << "(sp)" << std::endl;
                    stackOffset += type.GetTypeSize();
                    context.FreeTemporary(tempReg, stream);
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
