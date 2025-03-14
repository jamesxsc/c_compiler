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

    static std::map<std::set<TypeSpecifier::Type>, TypeSpecifier::Type> aliasMap{
            {{TypeSpecifier::UNSIGNED, TypeSpecifier::INT},  TypeSpecifier::UNSIGNED},
            {{TypeSpecifier::INT,      TypeSpecifier::INT},  TypeSpecifier::INT}, // signed int
            {{TypeSpecifier::UNSIGNED, TypeSpecifier::CHAR}, TypeSpecifier::CHAR},
    };

    TypeSpecifier ResolveTypeAlias(std::vector<TypeSpecifier> specifiers);

}
