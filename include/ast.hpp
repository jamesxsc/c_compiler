#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "ast_constant.hpp"
#include "ast_context.hpp"
#include "ast_function_declarator.hpp"
#include "ast_parameter_declaration.hpp"
#include "ast_parameter_list.hpp"
#include "ast_expression.hpp"
#include "ast_function_definition.hpp"
#include "ast_identifier.hpp"
#include "ast_jump_statement.hpp"
#include "ast_multiplicative_expression.hpp"
#include "ast_node.hpp"
#include "ast_postfix_expression.hpp"
#include "ast_primary_expression.hpp"
#include "ast_type_specifier.hpp"
#include "ast_unary_expression.hpp"

ast::NodePtr ParseAST(std::string file_name);
