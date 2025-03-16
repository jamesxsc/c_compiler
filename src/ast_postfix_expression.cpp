#include <cassert>
#include <cmath>
#include "ast_postfix_expression.hpp"
#include "ast_identifier.hpp"
#include "ast_type_specifier.hpp"
#include "ast_array_index_expression.hpp"

namespace ast {

    void PostfixExpression::EmitRISC(std::ostream &stream, Context &context, Register destReg) const {
        switch (op_) {
            case PostfixOperator::PrimaryPromote:
            case PostfixOperator::FunctionCallPromote:
            case PostfixOperator::ArrayIndexPromote:
                child_->EmitRISC(stream, context, destReg);
                break;
            case PostfixOperator::PostfixIncrement:
            case PostfixOperator::PostfixDecrement:
                std::string identifier = GetIdentifier();
                if (context.IsGlobal(identifier)) {
                    // Note this is slightly different from GCC because I want to delegate to child
                    // Store the incremented/decremented value in the destination register
                    if (destReg != Register::zero)
                        child_->EmitRISC(stream, context, destReg);
                    TypeSpecifier type = context.GetGlobalType(identifier);
                    switch (type) {
                        case TypeSpecifier::Type::INT:
                        case TypeSpecifier::Type::UNSIGNED:
                        case TypeSpecifier::Type::ENUM:
                        case TypeSpecifier::Type::CHAR: {
                            Register tempReg = context.AllocateTemporary();
                            Register tempReg2 = context.AllocateTemporary();
                            stream << "lui " << tempReg2 << ",%hi(" << identifier << ")" << std::endl;
                            stream << (type == TypeSpecifier::CHAR ? "lbu " : "lw ") << tempReg2 << ",%lo("
                                   << identifier << ")(" << tempReg2 << ")" << std::endl;
                            stream << "addi " << tempReg << "," << tempReg2 << ","
                                   << (op_ == PostfixOperator::PostfixIncrement ? "1" : "-1") << std::endl;
                            stream << "lui " << tempReg2 << ",%hi(" << identifier << ")" << std::endl;
                            stream << (type == TypeSpecifier::CHAR ? "sb " : "sw ") << tempReg << ",%lo(" << identifier
                                   << ")(" << tempReg2 << ")" << std::endl;
                            context.FreeTemporary(tempReg);
                            context.FreeTemporary(tempReg2);
                            break;
                        }
                        case TypeSpecifier::Type::FLOAT:
                        case TypeSpecifier::Type::DOUBLE: {
                            Register tempReg = context.AllocateTemporary();
                            Register tempFloatReg = context.AllocateTemporary(true);
                            stream << "lui " << tempReg << ",%hi(" << identifier << ")" << std::endl;
                            stream << (type == TypeSpecifier::Type::FLOAT ? "flw " : "fld ") << tempFloatReg << ",0("
                                   << tempReg
                                   << ")" << std::endl;
                            Register tempFloatReg2 = context.AllocateTemporary(true);
                            std::string constantMemoryLabel = context.MakeLabel("POSTFIX_CONSTANT");
                            stream << "lui " << tempReg << ",%hi(" << constantMemoryLabel << ")" << std::endl;
                            stream << (type == TypeSpecifier::Type::FLOAT ? "flw " : "fld ") << tempFloatReg2 << ",%lo("
                                   << constantMemoryLabel << ")(" << tempReg << ")" << std::endl;
                            if (op_ == PostfixOperator::PostfixIncrement) {
                                stream << (type == TypeSpecifier::Type::FLOAT ? "fadd.s " : "fadd.d ") << tempFloatReg
                                       << ","
                                       << tempFloatReg << "," << tempFloatReg2 << std::endl;
                            } else {
                                stream << (type == TypeSpecifier::Type::FLOAT ? "fsubd.s " : "fsub.d ") << tempFloatReg
                                       << ","
                                       << tempFloatReg << "," << tempFloatReg2 << std::endl;
                            }
                            stream << "lui " << tempReg << ",%hi(" << identifier << ")" << std::endl;
                            stream << (type == TypeSpecifier::Type::FLOAT ? "fsw " : "fsd ") << tempFloatReg << ",0("
                                   << tempReg
                                   << ")" << std::endl;
                            context.FreeTemporary(tempReg);
                            context.FreeTemporary(tempFloatReg);
                            context.FreeTemporary(tempFloatReg2);

                            context.DeferredRISC() << ".section .rodata" << std::endl;
                            context.DeferredRISC() << ".align " << (type == TypeSpecifier::Type::FLOAT ? 2 : 3)
                                                   << std::endl;
                            context.DeferredRISC() << constantMemoryLabel << ":" << std::endl;
                            context.DeferredRISC() << (type == TypeSpecifier::FLOAT ? ".float " : ".double ")
                                                   << (op_ == PostfixOperator::PostfixIncrement ? 1.0 : -1.0)
                                                   << std::endl;
                            break;
                        }
                        case TypeSpecifier::Type::POINTER:
                        case TypeSpecifier::Type::ARRAY: {
                            const TypeSpecifier &pointeeType = type.GetPointeeType();
                            Register tempReg = context.AllocateTemporary();
                            Register tempReg2 = context.AllocateTemporary();
                            stream << "lui " << tempReg2 << ",%hi(" << identifier << ")" << std::endl;
                            stream << "lw " << tempReg2 << ",%lo(" << identifier << ")(" << tempReg2 << ")"
                                   << std::endl;
                            int size = pointeeType.GetTypeSize();
                            stream << "addi " << tempReg << "," << tempReg2 << ","
                                   << (op_ == PostfixOperator::PostfixIncrement ? size : -size) << std::endl;
                            stream << "lui " << tempReg2 << ",%hi(" << identifier << ")" << std::endl;
                            stream << "sw " << tempReg << ",%lo(" << identifier << ")(" << tempReg2 << ")" << std::endl;
                            context.FreeTemporary(tempReg);
                            context.FreeTemporary(tempReg2);
                            break;
                        }
                        case TypeSpecifier::Type::VOID:
                        case TypeSpecifier::Type::STRUCT:
                            throw std::runtime_error(
                                    "PostfixExpression::EmitRISC() attempted to increment unsupported type");
                    }
                } else {
                    Variable variable = context.CurrentFrame().bindings.Get(GetIdentifier());
                    // Store the pre-inc/dec value in the destination register
                    if (destReg != Register::zero)
                        child_->EmitRISC(stream, context, destReg);

                    switch (variable.type) {
                        case TypeSpecifier::Type::INT:
                        case TypeSpecifier::Type::CHAR:
                        case TypeSpecifier::Type::UNSIGNED:
                        case TypeSpecifier::Type::ENUM: {
                            Register tempReg = context.AllocateTemporary();
                            stream << (variable.type == TypeSpecifier::CHAR ? "lbu " : "lw ") << tempReg << ","
                                   << variable.offset << "(s0)" << std::endl;
                            stream << "addi " << tempReg << "," << tempReg << ","
                                   << (op_ == PostfixOperator::PostfixIncrement ? 1 : -1) << std::endl;
                            stream << (variable.type == TypeSpecifier::CHAR ? "sb " : "sw ") << tempReg << ","
                                   << variable.offset << "(s0)" << std::endl;
                            context.FreeTemporary(tempReg);
                            break;
                        }
                        case TypeSpecifier::Type::FLOAT:
                        case TypeSpecifier::Type::DOUBLE: {
                            Register tempFloatReg = context.AllocateTemporary(true);
                            stream << (variable.type == TypeSpecifier::FLOAT ? "flw " : "fld ") << tempFloatReg << ","
                                   << variable.offset << "(s0)" << std::endl;
                            std::string constantMemoryLabel = context.MakeLabel("POSTFIX_CONSTANT");
                            Register tempReg = context.AllocateTemporary();
                            Register tempFloatReg2 = context.AllocateTemporary(true);
                            stream << "lui " << tempReg << ",%hi(" << constantMemoryLabel << ")" << std::endl;
                            stream << (variable.type == TypeSpecifier::FLOAT ? "flw " : "fld ") << tempFloatReg2
                                   << ",%lo("
                                   << constantMemoryLabel << ")(" << tempReg << ")" << std::endl;
                            if (op_ == PostfixOperator::PostfixIncrement) {
                                stream << (variable.type == TypeSpecifier::FLOAT ? "fadd.s " : "fadd.d ")
                                       << tempFloatReg
                                       << "," << tempFloatReg << "," << tempFloatReg2 << std::endl;
                            } else {
                                stream << (variable.type == TypeSpecifier::FLOAT ? "fsubd.s " : "fsub.d ")
                                       << tempFloatReg
                                       << "," << tempFloatReg << "," << tempFloatReg2 << std::endl;
                            }
                            stream << (variable.type == TypeSpecifier::FLOAT ? "fsw " : "fsd ") << tempFloatReg << ","
                                   << variable.offset << "(s0)" << std::endl;
                            context.FreeTemporary(tempReg);
                            context.FreeTemporary(tempFloatReg);
                            context.FreeTemporary(tempFloatReg2);

                            context.DeferredRISC() << ".section .rodata" << std::endl;
                            context.DeferredRISC() << ".align " << (variable.type == TypeSpecifier::Type::FLOAT ? 2 : 3)
                                                   << std::endl;
                            context.DeferredRISC() << constantMemoryLabel << ":" << std::endl;
                            context.DeferredRISC() << (variable.type == TypeSpecifier::Type::FLOAT ? ".float " : ".double ")
                                                   << (op_ == PostfixOperator::PostfixIncrement ? 1.0 : -1.0)
                                                   << std::endl;
                            break;
                        }
                        case TypeSpecifier::Type::POINTER:
                        case TypeSpecifier::Type::ARRAY: {
                            const TypeSpecifier &pointeeType = variable.type.GetPointeeType();
                            Register tempReg = context.AllocateTemporary();
                            stream << "lw " << tempReg << "," << variable.offset << "(s0)" << std::endl;
                            int size = pointeeType.GetTypeSize();
                            stream << "addi " << tempReg << "," << tempReg << ","
                                   << (op_ == PostfixOperator::PostfixIncrement ? size : -size) << std::endl;
                            stream << "sw " << tempReg << "," << variable.offset << "(s0)" << std::endl;
                            context.FreeTemporary(tempReg);
                            break;
                        }
                        case TypeSpecifier::Type::VOID:
                        case TypeSpecifier::Type::STRUCT:
                            throw std::runtime_error(
                                    "PostfixExpression::EmitRISC() attempted to increment unsupported type");
                    }
                }
                break;
        }
    }

    void PostfixExpression::Print(std::ostream &stream) const {
        switch (op_) {
            case PostfixOperator::PrimaryPromote:
            case PostfixOperator::FunctionCallPromote:
            case PostfixOperator::ArrayIndexPromote:
                child_->Print(stream);
                break;
            case PostfixOperator::PostfixIncrement:
                child_->Print(stream);
                stream << "++";
                break;
            case PostfixOperator::PostfixDecrement:
                child_->Print(stream);
                stream << "--";
                break;
        }
    }

    TypeSpecifier PostfixExpression::GetType(Context &context) const {
        return child_->GetType(context);
    }

    std::string PostfixExpression::GetIdentifier() const {
        switch (op_) {
            case PostfixOperator::PrimaryPromote:
            case PostfixOperator::FunctionCallPromote: {
                Identifier identifier = dynamic_cast<const Identifier &>(*child_);
                return identifier.GetIdentifier();
            }
            case PostfixOperator::PostfixIncrement:
            case PostfixOperator::PostfixDecrement: {
                // Child is a postfix expression
                return dynamic_cast<const PostfixExpression *>(child_.get())->GetIdentifier();
            }
            case PostfixOperator::ArrayIndexPromote:
                return dynamic_cast<const ArrayIndexExpression *>(child_.get())->GetIdentifier();
        }

        Identifier identifier = dynamic_cast<const Identifier &>(*child_);
        return identifier.GetIdentifier();
    }

    bool PostfixExpression::ContainsFunctionCall() const {
        return child_->ContainsFunctionCall();
    }

    std::string PostfixExpression::GetGlobalIdentifier() const {
        return child_->GetGlobalIdentifier();
    }

    const Expression &PostfixExpression::GetArrayIndexExpression() const {
        assert(op_ == PostfixOperator::ArrayIndexPromote &&
               "PostfixExpression::GetArrayIndexExpression called on non-array");
        return dynamic_cast<const ArrayIndexExpression *>(child_.get())->GetIndexExpression();
    }

    int PostfixExpression::Evaluate(Context &context) const {
        if (op_ != PostfixOperator::PrimaryPromote)
            throw std::runtime_error("PostfixExpression::Evaluate() called on non-primary expression");

        return child_->Evaluate(context);
    }

    double PostfixExpression::EvaluateFloat(ast::Context &context) const {
        if (op_ != PostfixOperator::PrimaryPromote)
            throw std::runtime_error("PostfixExpression::EvaluateFloat() called on non-primary expression");

        return child_->EvaluateFloat(context);
    }

} // namespace ast
