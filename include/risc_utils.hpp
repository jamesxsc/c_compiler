#pragma once

#include <iostream>
#include <map>
#include <set>

#include "ast_context.hpp"
#include "register.hpp"
#include "ast_expression_base.hpp"

namespace ast::Utils {

    void EmitComparison(std::ostream &stream, Context &context, Register result, const ExpressionBase &operand);

    void EmitMultiply(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                      const ExpressionBase &right);

    void EmitDivide(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                    const ExpressionBase &right);

    void EmitModulo(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                    const ExpressionBase &right);

    void EmitAddition(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                      const ExpressionBase &right);

    void EmitSubtraction(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                         const ExpressionBase &right);

    void EmitLeftShift(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                       const ExpressionBase &right);

    void EmitRightShift(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                        const ExpressionBase &right);

    void EmitBitwiseAnd(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                        const ExpressionBase &right);

    void EmitBitwiseXor(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                        const ExpressionBase &right);

    void EmitBitwiseOr(std::ostream &stream, Context &context, Register result, const ExpressionBase &left,
                       const ExpressionBase &right);

    TypeSpecifier BinaryResultType(const TypeSpecifier &leftType, const TypeSpecifier &rightType);

    TypeSpecifier BinaryAdditionResultType(const TypeSpecifier& leftType, const TypeSpecifier& rightType);

    TypeSpecifier BinarySubtractionResultType(const TypeSpecifier& leftType, const TypeSpecifier& rightType);

    void EmitIndexToAddressOffset(std::ostream &stream, Register sizeReg, Context &context, const TypeSpecifier& type);

    void EmitAddressToIndexOffset(std::ostream &stream, Register sizeReg, Context &context, const TypeSpecifier& type);

    void EmitIncrementDecrement(std::ostream &stream, Context &context, Register destReg, const ExpressionBase &child, bool decrement, bool postfix);

}
