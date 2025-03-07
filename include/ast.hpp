#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "ast_additive_expression.hpp"
#include "ast_and_expression.hpp"
#include "ast_argument_expression_list.hpp"
#include "ast_assignment_expression.hpp"
#include "ast_compound_statement.hpp"
#include "ast_conditional_expression.hpp"
#include "ast_constant.hpp"
#include "ast_constant_expression.hpp"
#include "ast_context.hpp"
#include "ast_declaration.hpp"
#include "ast_declaration_list.hpp"
#include "ast_declaration_specifiers.hpp"
#include "ast_declarator.hpp"
#include "ast_equality_expression.hpp"
#include "ast_exclusive_or_expression.hpp"
#include "ast_function_declarator.hpp"
#include "ast_parameter_declaration.hpp"
#include "ast_parameter_list.hpp"
#include "ast_parenthesised_expression.hpp"
#include "ast_pointer_declarator.hpp"
#include "ast_expression.hpp"
#include "ast_expression_base.hpp"
#include "ast_expression_statement.hpp"
#include "ast_function_call_expression.hpp"
#include "ast_function_definition.hpp"
#include "ast_identifier.hpp"
#include "ast_inclusive_or_expression.hpp"
#include "ast_init_declarator.hpp"
#include "ast_init_declarator_list.hpp"
#include "ast_initializer.hpp"
#include "ast_iteration_statement.hpp"
#include "ast_jump_statement.hpp"
#include "ast_labeled_statement.hpp"
#include "ast_logical_and_expression.hpp"
#include "ast_logical_or_expression.hpp"
#include "ast_multiplicative_unary_expressions.hpp"
#include "ast_node.hpp"
#include "ast_postfix_expression.hpp"
#include "ast_primary_expression.hpp"
#include "ast_type_specifier.hpp"
#include "ast_relational_expression.hpp"
#include "ast_selection_statement.hpp"
#include "ast_shift_expression.hpp"
#include "ast_statement.hpp"
#include "ast_statement_list.hpp"

ast::NodePtr ParseAST(std::string file_name);
