#include "ast_identifier.hpp"
#include "ast_type_specifier.hpp"
#include "cassert"

namespace ast {


    void Identifier::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        // If we are in LHS, load address
        if (context.EmitLHS()) {
            if (context.IsGlobal(identifier_)) {
                stream << "lui " << destReg << ",%hi(" << identifier_ << ")" << std::endl;
                stream << "addi " << destReg << "," << destReg << ",%lo(" << identifier_ << ")" << std::endl;

                if (GetType(context) == TypeSpecifier::POINTER && context.dereference) { // todo array?
                    // todo recursion and struct case outside of emitlhs
                    stream << "lw " << destReg << ",0(" << destReg << ")" << std::endl;
                }
            } else if (context.CurrentFrame().bindings.Contains(identifier_)) {
                int offset = context.CurrentFrame().bindings.Get(identifier_).offset;
                stream << "addi " << destReg << ",s0," << offset << std::endl;

                if (GetType(context) == TypeSpecifier::POINTER && context.dereference) {
                    stream << "lw " << destReg << ",0(" << destReg << ")" << std::endl;
                }
            } else {
                throw std::runtime_error("Identifier::EmitRISC() (LHS) called on an undeclared identifier");
            }
            return;
        }

        // Otherwise load value
        if (context.IsGlobal(identifier_)) {
            TypeSpecifier type = GetType(context);
            switch (type) {
                case TypeSpecifier::DOUBLE:
                case TypeSpecifier::FLOAT: {
                    assert(IsFloatRegister(destReg) &&
                           "Identifier::EmitRISC attempting to load float into integer register");
                    Register tempIntReg = context.AllocateTemporary(stream);
                    stream << "lui " << tempIntReg << ",%hi(" << identifier_ << ")" << std::endl;
                    stream << (type == TypeSpecifier::FLOAT ? "flw " : "fld ") << destReg << ",%lo(" << identifier_
                           << ")(" << tempIntReg << ")" << std::endl;
                    context.FreeTemporary(tempIntReg, stream);
                    break;
                }
                case TypeSpecifier::POINTER:
                case TypeSpecifier::ENUM:
                case TypeSpecifier::INT:
                case TypeSpecifier::UNSIGNED:
                    assert(!IsFloatRegister(destReg) &&
                           "Identifier::EmitRISC attempting to load non-float into float register");
                    stream << "lui " << destReg << ",%hi(" << identifier_ << ")" << std::endl;
                    stream << "lw " << destReg << ",%lo(" << identifier_ << ")(" << destReg << ")" << std::endl;
                    break;
                case TypeSpecifier::CHAR:
                    stream << "lui " << destReg << ",%hi(" << identifier_ << ")" << std::endl;
                    stream << "lbu " << destReg << ",%lo(" << identifier_ << ")(" << destReg << ")" << std::endl;
                    break;
                case TypeSpecifier::ARRAY:
                    // Load address
                    stream << "lui " << destReg << ",%hi(" << identifier_ << ")" << std::endl;
                    stream << "addi " << destReg << "," << destReg << ",%lo(" << identifier_ << ")" << std::endl;
                    break;
                case TypeSpecifier::STRUCT:
                    // do this
                case TypeSpecifier::VOID:
                    throw std::runtime_error(
                            "Identifier::EmitRISC() called on an unsupported type");
            }
        } else if (context.CurrentFrame().bindings.Contains(identifier_)) {
            TypeSpecifier type = GetType(context);
            int offset = context.CurrentFrame().bindings.Get(identifier_).offset;
            switch (type) {
                case TypeSpecifier::FLOAT:
                case TypeSpecifier::DOUBLE:
                    assert(IsFloatRegister(destReg) &&
                           "Identifier::EmitRISC attempting to load double into integer register");
                    stream << (type == TypeSpecifier::FLOAT ? "flw " : "fld ") << destReg << "," << offset << "(s0)"
                           << std::endl;
                    break;
                case TypeSpecifier::POINTER:
                case TypeSpecifier::INT:
                case TypeSpecifier::UNSIGNED:
                case TypeSpecifier::ENUM:
                    assert(!IsFloatRegister(destReg) &&
                           "Identifier::EmitRISC attempting to load non-float into float register");
                    stream << "lw " << destReg << "," << offset << "(s0)" << std::endl;
                    break;
                case TypeSpecifier::CHAR:
                    stream << "lbu " << destReg << "," << offset << "(s0)" << std::endl;
                    break;
                case TypeSpecifier::ARRAY:
                    // Load address, but probably never called in local scope
                    stream << "addi " << destReg << ",s0," << offset << std::endl;
                    break;
                case TypeSpecifier::STRUCT: { // Should only ever be called in return
                    assert(destReg == Register::a0 && "Structs identifier called in non-return context");
                    const std::vector<std::pair<std::string, TypeSpecifier>> &structMembers = type.GetStructMembers();
                    // I think this is best placed here because I can't think where else we want it
                    if (type.UseStack()) {
                        // Get hidden pointer
                        int address = context.CurrentFrame().bindings.Get("#hiddenpointer").offset;
                        Register addressReg = context.AllocateTemporary(stream);
                        stream << "lw " << addressReg << "," << address << "(s0)" << std::endl;

                        // Store members, don't care about types, keep padding
                        int size = type.GetTypeSize();
                        Register tempReg = context.AllocateTemporary(stream);
                        for (int i = 0; i < size; i += 4) {
                            stream << "lw " << tempReg << "," << offset + i << "(s0)" << std::endl;
                            stream << "sw " << tempReg << "," << i << "(" << addressReg << ")" << std::endl;
                        }
                        context.FreeTemporary(tempReg, stream);

                        // Return address
                        stream << "mv a0," << addressReg << std::endl;
                        context.FreeTemporary(addressReg, stream);
                    } else {
                        Register floatReg = Register::fa0;
                        Register intReg = Register::a0;
                        int memberOffset = 0;
                        for (const auto &member: structMembers) {
                            TypeSpecifier memberType = member.second;
                            if (member.first == "#padding") { // Don't store padding
                                offset += memberType.GetTypeSize();
                                continue;
                            }
                            switch (memberType) {
                                case TypeSpecifier::Type::INT:
                                case TypeSpecifier::Type::CHAR:
                                case TypeSpecifier::Type::UNSIGNED:
                                case TypeSpecifier::Type::POINTER:
                                case TypeSpecifier::Type::ENUM:
                                    stream << "lw " << intReg << "," << offset + memberOffset << "(s0)" << std::endl;
                                    memberOffset += memberType.GetTypeSize();
                                    intReg = static_cast<Register>(static_cast<int>(intReg) + 1);
                                    break;
                                case TypeSpecifier::Type::FLOAT:
                                case TypeSpecifier::Type::DOUBLE:
                                    stream << (memberType == TypeSpecifier::FLOAT ? "flw " : "fld ") << floatReg << ","
                                           << offset + memberOffset << "(s0)" << std::endl;
                                    memberOffset += memberType.GetTypeSize();
                                    floatReg = static_cast<Register>(static_cast<int>(floatReg) + 1);
                                    break;
                                case TypeSpecifier::Type::ARRAY:
                                case TypeSpecifier::Type::VOID:
                                case TypeSpecifier::Type::STRUCT:
                                    throw std::runtime_error(
                                            "ReturnStatement::EmitRISC() called on an unsupported struct member type");
                            }
                        }
                    }
                    break;
                }
                case TypeSpecifier::VOID:
                    throw std::runtime_error(
                            "Identifier::EmitRISC() called on an unsupported type");
            }
        } else if (context.IsEnum(identifier_)) { // Enumerator identifier
            int value = context.LookupEnum(identifier_);
            stream << "li " << destReg << "," << value << std::endl;
        } else {
            throw std::runtime_error("Identifier::EmitRISC() called on an undeclared identifier");
        }
    }

    void Identifier::Print(std::ostream &stream) const {
        stream << identifier_;
    }

    const std::string &Identifier::GetIdentifier() const {
        return identifier_;
    }

    TypeSpecifier Identifier::GetType(Context &context) const {
        if (context.IsGlobal(identifier_)) {
            return context.GetGlobalType(identifier_);
        } else if (context.CurrentFrame().bindings.Contains(identifier_)) {
            Variable var = context.CurrentFrame().bindings.Get(identifier_);
            return var.type;
        } else if (context.IsEnum(identifier_)) {
            return TypeSpecifier{identifier_};
        } else {
            // Nice no more sigfaults
            throw std::runtime_error("Identifier::GetType() called on an undeclared identifier");
        }
    }

    bool Identifier::ContainsFunctionCall() const {
        return false;
    }

    std::string Identifier::GetGlobalIdentifier() const {
        return identifier_;
    }

    int Identifier::Evaluate(Context &context) const {
        // Enums can be used here
        if (context.IsEnum(identifier_)) {
            return context.LookupEnum(identifier_);
        }

        throw std::runtime_error("Identifier::Evaluate() called on a non-enum identifier");
    }

    double Identifier::EvaluateFloat(ast::Context &context) const {
        throw std::runtime_error("Identifier::EvaluateFloat() called on an identifier");
    }

}
