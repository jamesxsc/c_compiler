#pragma once

#include <iostream>

#include "ast_context.hpp"
#include "register.hpp"
#include "ast_expression_base.hpp"

namespace ast::Utils {

    void EmitComparison(std::ostream &ostream, Context &context, Register result, const ExpressionBase &operand);

}
