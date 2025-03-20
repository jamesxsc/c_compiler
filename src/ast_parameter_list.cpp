#include "ast_parameter_list.hpp"
#include "ast_type_specifier.hpp"

namespace ast {

    ParameterList::ParameterList(ParameterDeclarationPtr first) : parameters_() {
        if (first)
            parameters_.push_back(std::move(first));
    }

    // todo some extraction and simplification throughout - this for example is getting a bit crazy

    void ParameterList::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        if (parameters_.empty() && !hiddenPointerReturn_) {
            return;
        }

        int iidx = 0, fidx = 0;
        if (hiddenPointerReturn_) { // Always in first register
            Variable var = context.CurrentFrame().bindings.Insert("#hiddenpointer", Variable{
                    .size = 4,
                    .type = TypeSpecifier::POINTER // Note that the pointee type is not stored
            });
            stream << "sw a" << iidx << "," << var.offset << "(s0)" << std::endl;
            ++iidx;
        }
        int stackOffset = 0; // Positive offset into previous frame
        for (const auto &param: parameters_) {
            TypeSpecifier type = param->GetType(context);
            // NOTE: GCC passes structs by value despite the ABI stating otherwise, we match GCC

            // note that struct / aggregate using stack is different to other things using stack
            // that is just because its big
            // this use stack is when we're out of registers, then we store ptrs in previous frame
            bool noFloatRegs = fidx > 7;
            bool noIntRegs = iidx > 7; // todo maybe consider a struct that will half fit?

            bool used = param->HasIdentifier();
            Variable var = used ?
                           context.CurrentFrame().bindings.Insert(param->GetIdentifier(), Variable{
                                   .size = type.GetTypeSize(),
                                   .type = type
                           }) :
                           Variable{ // Don't store
                                   .size = type.GetTypeSize(),
                                   .type = type
                                   // Offset not used
                           };
            switch (type) {
                case TypeSpecifier::FLOAT:
                case TypeSpecifier::DOUBLE:
                    if (noFloatRegs) {
                        if (used) {
                            Register tempReg = context.AllocateTemporary(stream, true);
                            stream << (type == TypeSpecifier::FLOAT ? "flw " : "fld ") << tempReg << ","
                                   << stackOffset << "(s0)" << std::endl;
                            stream << (type == TypeSpecifier::FLOAT ? "fsw " : "fsd ") << tempReg << ","
                                   << var.offset << "(s0)" << std::endl; // Copy
                            context.FreeTemporary(tempReg, stream);
                        }
                        stackOffset += (type == TypeSpecifier::FLOAT ? 4 : 8);
                    } else {
                        if (used) // Have to still incr counters
                            stream << (type == TypeSpecifier::FLOAT ? "fsw " : "fsd ")
                                   << "fa" << fidx << "," << var.offset << "(s0)" << std::endl;
                        ++fidx;
                    }
                    break;
                case TypeSpecifier::POINTER:
                case TypeSpecifier::INT:
                case TypeSpecifier::ENUM:
                case TypeSpecifier::UNSIGNED:
                    if (noIntRegs) {
                        if (used) {
                            Register tempReg = context.AllocateTemporary(stream);
                            stream << "lw " << tempReg << "," << stackOffset << "(s0)" << std::endl;
                            stream << "sw " << tempReg << "," << var.offset << "(s0)" << std::endl; // Copy
                            context.FreeTemporary(tempReg, stream);
                        }
                        stackOffset += 4;
                    } else {
                        if (used)
                            stream << "sw a" << iidx << "," << var.offset << "(s0)" << std::endl;
                        ++iidx;
                    }
                    break;
                case TypeSpecifier::CHAR:
                    if (noIntRegs) {
                        if (used) {
                            Register tempReg = context.AllocateTemporary(stream);
                            stream << "lbu " << tempReg << "," << stackOffset << "(s0)" << std::endl;
                            stream << "sb " << tempReg << "," << var.offset << "(s0)" << std::endl; // Copy
                            context.FreeTemporary(tempReg, stream);
                        }
                        stackOffset += 1;
                    } else {
                        if (used)
                            stream << "sb a" << iidx << "," << var.offset << "(s0)" << std::endl;
                        ++iidx;
                    }
                    break;
                case TypeSpecifier::STRUCT:
                    EmitStructParameter(type, stream, context, used, noIntRegs, noFloatRegs, var.offset, iidx, fidx,
                                        stackOffset, type.UseStack());
                    break;
                case TypeSpecifier::VOID:
                case TypeSpecifier::ARRAY:
                    throw std::runtime_error(
                            "ParameterList::EmitRISC() called on an unsupported array type");
            }
        }
    }

    void ParameterList::EmitStructParameter(const TypeSpecifier &type, std::ostream &stream, Context &context, bool used,
                                            bool noIntRegs, bool noFloatRegs, int baseOffset, int &iidx, int &fidx,
                                            int &stackOffset, bool useStack) const {
        int memberOffset = 0;
        for (const auto &member: type.GetStructMembers()) {
            if (member.first == "#padding") {
                memberOffset += member.second.GetTypeSize();
                continue;
            }
            switch (member.second) {
                case TypeSpecifier::Type::INT:
                case TypeSpecifier::Type::UNSIGNED:
                case TypeSpecifier::Type::POINTER:
                case TypeSpecifier::Type::ARRAY: // todo array recursion
                case TypeSpecifier::Type::ENUM:
                    if (useStack && used) {
                        Register tempReg = context.AllocateTemporary(stream);
                        // Load from previous frame
                        if (noIntRegs) {
                            // Get ptr
                            stream << "lw " << tempReg << "," << stackOffset << "(s0)" << std::endl;
                            stream << "lw " << tempReg << "," << memberOffset << "(" << tempReg << ")"
                                   << std::endl;
                        } else {
                            stream << "lw " << tempReg << "," << memberOffset << "(a" << iidx << ")"
                                   << std::endl;
                        }
                        stream << "sw " << tempReg << "," << baseOffset + memberOffset << "(s0)"
                               << std::endl;
                        // Don't increment iidx
                        context.FreeTemporary(tempReg, stream);
                    } else {
                        if (noIntRegs) {
                            if (used) {
                                Register tempReg = context.AllocateTemporary(stream);
                                stream << "lw " << tempReg << "," << stackOffset << "(s0)" << std::endl;
                                stream << "sw " << tempReg << "," << baseOffset + memberOffset << "(s0)"
                                       << std::endl;
                                context.FreeTemporary(tempReg, stream);
                            }
                            stackOffset += 4;
                        } else {
                            if (used)
                                stream << "sw a" << iidx << "," << baseOffset + memberOffset << "(s0)"
                                       << std::endl;
                            ++iidx;
                        }
                    }
                    break;
                case TypeSpecifier::Type::CHAR:
                    if (useStack && used) {
                        Register tempReg = context.AllocateTemporary(stream);
                        if (noIntRegs) {
                            stream << "lw " << tempReg << "," << stackOffset << "(s0)" << std::endl;
                            stream << "lbu " << tempReg << "," << memberOffset << "(" << tempReg << ")"
                                   << std::endl;
                        } else {
                            stream << "lbu " << tempReg << "," << memberOffset << "(a" << iidx << ")"
                                   << std::endl;
                        }
                        stream << "sb " << tempReg << "," << baseOffset + memberOffset << "(s0)"
                               << std::endl;
                        context.FreeTemporary(tempReg, stream);
                    } else {
                        if (noIntRegs) {
                            if (used) {
                                Register tempReg = context.AllocateTemporary(stream);
                                stream << "lbu " << tempReg << "," << stackOffset << "(s0)" << std::endl;
                                stream << "sb " << tempReg << "," << baseOffset + memberOffset << "(s0)"
                                       << std::endl;
                                context.FreeTemporary(tempReg, stream);
                            }
                            stackOffset += 1;
                        } else {
                            if (used)
                                stream << "sb a" << iidx << "," << baseOffset + memberOffset << "(s0)"
                                       << std::endl;
                            ++iidx;
                        }
                    }
                    break;
                case TypeSpecifier::Type::FLOAT:
                case TypeSpecifier::Type::DOUBLE:
                    if (useStack && used) {
                        Register tempReg = context.AllocateTemporary(stream, true);
                        if (noIntRegs) {
                            stream << "lw " << tempReg << "," << stackOffset << "(s0)" << std::endl;
                            stream << (member.second == TypeSpecifier::FLOAT ? "flw " : "fld ") << tempReg
                                   << "," << memberOffset << "(" << tempReg << ")" << std::endl;
                        } else {
                            stream << (member.second == TypeSpecifier::FLOAT ? "flw " : "fld ") << tempReg
                                   << "," << memberOffset << "(a" << iidx << ")" << std::endl;
                        }
                        stream << (member.second == TypeSpecifier::FLOAT ? "fsw " : "fsd ") << tempReg
                               << "," << baseOffset + memberOffset << "(s0)" << std::endl;
                        context.FreeTemporary(tempReg, stream);
                    } else {
                        if (noFloatRegs) {
                            if (used) {
                                Register tempReg = context.AllocateTemporary(stream, true);
                                stream << (member.second == TypeSpecifier::FLOAT ? "flw " : "fld ")
                                       << tempReg
                                       << "," << stackOffset << "(s0)" << std::endl;
                                stream << (member.second == TypeSpecifier::FLOAT ? "fsw " : "fsd ")
                                       << tempReg
                                       << "," << baseOffset + memberOffset << "(s0)" << std::endl;
                                context.FreeTemporary(tempReg, stream);
                            }
                            stackOffset += (member.second == TypeSpecifier::FLOAT ? 4 : 8);
                        } else {
                            if (used)
                                stream << (member.second == TypeSpecifier::FLOAT ? "fsw " : "fsd ")
                                       << "fa" << fidx << "," << baseOffset + memberOffset << "(s0)"
                                       << std::endl;
                            ++fidx;
                        }
                    }
                    break;
                case TypeSpecifier::Type::STRUCT:
                    EmitStructParameter(member.second, stream, context, used, noIntRegs, noFloatRegs,
                                        baseOffset + memberOffset, iidx, fidx, stackOffset, useStack);
                    memberOffset += member.second.GetTypeSize();
                    break;
                case TypeSpecifier::Type::VOID:
                    throw std::runtime_error(
                            "ParameterList::EmitRISC() called on an unsupported struct member type");
            }
            memberOffset += member.second.GetTypeSize();
        }
        if (useStack) {
            if (noIntRegs)
                stackOffset += 4; // Now we are done with the stack ptr
            else
                ++iidx; // Now we are done with the reg holding ptr
        }
    }

    void ParameterList::Print(std::ostream &stream) const {
        // Never called
    }

    void ParameterList::PushBack(ParameterDeclarationPtr item) {
        parameters_.push_back(std::move(item));
    }

    size_t ParameterList::Size() const {
        return parameters_.size();
    }

    std::vector<ParameterDeclarationPtr>::const_iterator ParameterList::begin() const {
        return parameters_.begin();
    }

    std::vector<ParameterDeclarationPtr>::const_iterator ParameterList::end() const {
        return parameters_.end();
    }

    void ParameterList::SetHiddenPointerReturn() const {
        hiddenPointerReturn_ = true;
    }

}
