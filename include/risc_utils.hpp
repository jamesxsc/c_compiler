#pragma once

#include <iostream>
#include <map>
#include <set>

#include "ast_context.hpp"
#include "register.hpp"
#include "ast_expression_base.hpp"

namespace ast::Utils {

    void EmitComparison(std::ostream &ostream, Context &context, Register result, const ExpressionBase &operand);

    static std::map<std::set<TypeSpecifier::Type>, TypeSpecifier::Type> aliasMap {
            {{TypeSpecifier::UNSIGNED, TypeSpecifier::INT}, TypeSpecifier::UNSIGNED},
            {{TypeSpecifier::INT, TypeSpecifier::INT}, TypeSpecifier::INT}, // signed int
            {{TypeSpecifier::UNSIGNED, TypeSpecifier::CHAR}, TypeSpecifier::CHAR},
    };

    TypeSpecifier ResolveTypeAlias(std::vector<TypeSpecifier> specifiers);

}
