// Adapted from: https://www.lysator.liu.se/c/ANSI-C-grammar-y.html

%code requires{
    #include "ast.hpp"
    #include <cassert>

	using namespace ast;

    extern Node* g_root;
    extern FILE* yyin;
    int yylex(void);
    void yyerror(const char*);
	int yylex_destroy(void);
}

// Represents the value associated with any kind of AST node.
%union{
  Node*        	node;
  NodeList*    	node_list;
  ExpressionBase* expression_base;
  PostfixExpression* postfix_expression;
  UnaryExpression* unary_expression; UnaryOperator unary_operator;
  MultiplicativeExpression* multiplicative_expression;
  AdditiveExpression* additive_expression;
  ShiftExpression* shift_expression;
  RelationalExpression* relational_expression;
  EqualityExpression* equality_expression;
  AndExpression* and_expression;
  ExclusiveOrExpression* exclusive_or_expression;
  InclusiveOrExpression* inclusive_or_expression;
  LogicalAndExpression* logical_and_expression;
  LogicalOrExpression* logical_or_expression;
  ConditionalExpression* conditional_expression;
  AssignmentExpression* assignment_expression; AssignmentOperator assignment_operator;
  Expression*  	expression;
  ParameterDeclaration*     parameter_declaration;
  Declarator*         direct_declarator;
  InitDeclarator*     init_declarator;
  ParameterList* parameter_list;
  int          	number_int;
  double       	number_float;
  std::string* 	string;
  TypeSpecifier	type_specifier;
  yytokentype  	token;
}

%token IDENTIFIER INT_CONSTANT FLOAT_CONSTANT STRING_LITERAL
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP AND_OP OR_OP
%token MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token TYPE_NAME TYPEDEF EXTERN STATIC AUTO REGISTER SIZEOF
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

// Note: Prefer %nterm over %type

%type <node> translation_unit external_declaration function_definition
%type <expression_base> primary_expression argument_expression_list

%type <expression_base> constant_expression
%type <node> declaration init_declarator_list
%type <node> struct_specifier struct_declaration_list struct_declaration specifier_qualifier_list struct_declarator_list
%type <node> struct_declarator enum_specifier enumerator_list enumerator pointer
%type <node> identifier_list type_name abstract_declarator direct_abstract_declarator initializer initializer_list statement labeled_statement
%type <node> compound_statement declaration_list expression_statement selection_statement iteration_statement jump_statement

%nterm <node_list> statement_list
%nterm <parameter_list> parameter_list

%nterm <direct_declarator> direct_declarator declarator // Differentiated by a bool member
%nterm <init_declarator> init_declarator
%nterm <parameter_declaration> parameter_declaration

// Expression hierachy
%nterm <postfix_expression> postfix_expression
%nterm <unary_expression> unary_expression
%nterm <unary_operator> unary_operator
%nterm <multiplicative_expression> multiplicative_expression
%nterm <additive_expression> additive_expression
%nterm <shift_expression> shift_expression
%nterm <relational_expression> relational_expression
%nterm <equality_expression> equality_expression
%nterm <and_expression> and_expression
%nterm <exclusive_or_expression> exclusive_or_expression
%nterm <inclusive_or_expression> inclusive_or_expression
%nterm <logical_and_expression> logical_and_expression
%nterm <logical_or_expression> logical_or_expression
%nterm <conditional_expression> conditional_expression
%nterm <assignment_expression> assignment_expression
%nterm <assignment_operator> assignment_operator
%nterm <expression> expression

%type <string> storage_class_specifier

%type <number_int> INT_CONSTANT STRING_LITERAL
%type <number_float> FLOAT_CONSTANT
%type <string> IDENTIFIER
// Cx these two; removed from lists above
%type <type_specifier> type_specifier
// TODO: Make a better type for this (only needed for advanced features)
%type <type_specifier> declaration_specifiers


%start ROOT
%%

ROOT
  : translation_unit { g_root = $1; }

translation_unit
	: external_declaration { $$ = $1; }
	| translation_unit external_declaration
	;

external_declaration
	: function_definition { $$ = $1; }
	| declaration
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement
	| declaration_specifiers declarator compound_statement {
	    // Check we didn't somehow get a non-function declarator
	    auto * decl = dynamic_cast<FunctionDeclarator*>($2);
	    assert(decl != nullptr && "Expected a function declarator in function_definition");
		$$ = new FunctionDefinition($1, FunctionDeclaratorPtr(decl), NodePtr($3));
	}
	| declarator declaration_list compound_statement
	| declarator compound_statement {
	    std::cerr << "Unimplemented function definition grammar" << std::endl;
	}
	;


primary_expression
	: IDENTIFIER {
	    $$ = new Identifier(*$1);
	}
	| INT_CONSTANT {
		$$ = new IntConstant($1);
	}
    | FLOAT_CONSTANT
	| STRING_LITERAL
	| '(' expression ')'
	;

postfix_expression
	: primary_expression { $$ = new PostfixExpression(ExpressionBasePtr($1)); }
	| postfix_expression '[' expression ']'
	| postfix_expression '(' ')'
	| postfix_expression '(' argument_expression_list ')'
	| postfix_expression '.' IDENTIFIER
	| postfix_expression PTR_OP IDENTIFIER
	| postfix_expression INC_OP
	| postfix_expression DEC_OP
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression
	: postfix_expression { $$ = new UnaryExpression(PostfixExpressionPtr($1)); }
	| INC_OP unary_expression
	| DEC_OP unary_expression
//	| unary_operator cast_expression // Casts are not required to be implemented
	| unary_operator multiplicative_expression
	| SIZEOF unary_expression
	| SIZEOF '(' type_name ')'
	;

unary_operator
	: '&' { $$ = UnaryOperator::AddressOf; }
	| '*' { $$ = UnaryOperator::Dereference; }
	| '+' { $$ = UnaryOperator::Plus; }
	| '-' { $$ = UnaryOperator::Minus; }
	| '~' { $$ = UnaryOperator::BitwiseNot; }
	| '!' { $$ = UnaryOperator::LogicalNot; }
	;

//cast_expression
//	: unary_expression
//	| '(' type_name ')' cast_expression
//	;
//multiplicative_expression
//	: cast_expression
//	| multiplicative_expression '*' cast_expression
//	| multiplicative_expression '/' cast_expression
//	| multiplicative_expression '%' cast_expression
//	;
// Casts are not required to be implemented
multiplicative_expression
    : unary_expression { $$ = new MultiplicativeExpression(UnaryExpressionPtr($1)); }
    | multiplicative_expression '*' unary_expression { $$ = new MultiplicativeExpression(MultiplicativeExpressionPtr($1), UnaryExpressionPtr($3), MultiplicativeOperator::Multiply); }
    | multiplicative_expression '/' unary_expression { $$ = new MultiplicativeExpression(MultiplicativeExpressionPtr($1), UnaryExpressionPtr($3), MultiplicativeOperator::Divide); }
    | multiplicative_expression '%' unary_expression { $$ = new MultiplicativeExpression(MultiplicativeExpressionPtr($1), UnaryExpressionPtr($3), MultiplicativeOperator::Modulo); }
    ;

additive_expression
	: multiplicative_expression { $$ = new AdditiveExpression(MultiplicativeExpressionPtr($1)); }
	| additive_expression '+' multiplicative_expression { $$ = new AdditiveExpression(AdditiveExpressionPtr($1), MultiplicativeExpressionPtr($3), AdditiveOperator::Add); }
	| additive_expression '-' multiplicative_expression { $$ = new AdditiveExpression(AdditiveExpressionPtr($1), MultiplicativeExpressionPtr($3), AdditiveOperator::Subtract); }
	;

shift_expression
	: additive_expression { $$ = new ShiftExpression(AdditiveExpressionPtr($1)); }
	| shift_expression LEFT_OP additive_expression { $$ = new ShiftExpression(ShiftExpressionPtr($1), AdditiveExpressionPtr($3), ShiftOperator::Left); }
	| shift_expression RIGHT_OP additive_expression { $$ = new ShiftExpression(ShiftExpressionPtr($1), AdditiveExpressionPtr($3), ShiftOperator::Right); }
	;

relational_expression
	: shift_expression { $$ = new RelationalExpression(ShiftExpressionPtr($1)); }
	| relational_expression '<' shift_expression { $$ = new RelationalExpression(RelationalExpressionPtr($1), ShiftExpressionPtr($3), RelationalOperator::LessThan); }
	| relational_expression '>' shift_expression { $$ = new RelationalExpression(RelationalExpressionPtr($1), ShiftExpressionPtr($3), RelationalOperator::GreaterThan); }
	| relational_expression LE_OP shift_expression { $$ = new RelationalExpression(RelationalExpressionPtr($1), ShiftExpressionPtr($3), RelationalOperator::LessThanOrEqual); }
	| relational_expression GE_OP shift_expression { $$ = new RelationalExpression(RelationalExpressionPtr($1), ShiftExpressionPtr($3), RelationalOperator::GreaterThanOrEqual); }
	;

equality_expression
	: relational_expression { $$ = new EqualityExpression(RelationalExpressionPtr($1)); }
	| equality_expression EQ_OP relational_expression { $$ = new EqualityExpression(EqualityExpressionPtr($1), RelationalExpressionPtr($3), EqualityOperator::Equality); }
	| equality_expression NE_OP relational_expression { $$ = new EqualityExpression(EqualityExpressionPtr($1), RelationalExpressionPtr($3), EqualityOperator::Inequality); }
	;

and_expression
	: equality_expression { $$ = new AndExpression(EqualityExpressionPtr($1)); }
	| and_expression '&' equality_expression { $$ = new AndExpression(AndExpressionPtr($1), EqualityExpressionPtr($3)); }
	;

exclusive_or_expression
	: and_expression { $$ = new ExclusiveOrExpression(AndExpressionPtr($1)); }
	| exclusive_or_expression '^' and_expression { $$ = new ExclusiveOrExpression(ExclusiveOrExpressionPtr($1), AndExpressionPtr($3)); }
	;

inclusive_or_expression
	: exclusive_or_expression { $$ = new InclusiveOrExpression(ExclusiveOrExpressionPtr($1)); }
	| inclusive_or_expression '|' exclusive_or_expression { $$ = new InclusiveOrExpression(InclusiveOrExpressionPtr($1), ExclusiveOrExpressionPtr($3)); }
	;

logical_and_expression
	: inclusive_or_expression { $$ = new LogicalAndExpression(InclusiveOrExpressionPtr($1)); }
	| logical_and_expression AND_OP inclusive_or_expression { $$ = new LogicalAndExpression(LogicalAndExpressionPtr($1), InclusiveOrExpressionPtr($3)); }
	;

logical_or_expression
	: logical_and_expression { $$ = new LogicalOrExpression(LogicalAndExpressionPtr($1)); }
	| logical_or_expression OR_OP logical_and_expression { $$ = new LogicalOrExpression(LogicalOrExpressionPtr($1), LogicalAndExpressionPtr($3)); }
	;

conditional_expression
	: logical_or_expression { $$ = new ConditionalExpression(LogicalOrExpressionPtr($1)); }
	| logical_or_expression '?' expression ':' conditional_expression { $$ = new ConditionalExpression(LogicalOrExpressionPtr($1), ExpressionPtr($3), ConditionalExpressionPtr($5)); }
	;

assignment_expression
	: conditional_expression { $$ = new AssignmentExpression(ConditionalExpressionPtr($1)); }
	| unary_expression assignment_operator assignment_expression { $$ = new AssignmentExpression(UnaryExpressionPtr($1), $2, AssignmentExpressionPtr($3)); }
	;

assignment_operator
	: '=' { $$ = AssignmentOperator::Assign; }
	| MUL_ASSIGN { $$ = AssignmentOperator::MultiplyAssign; }
	| DIV_ASSIGN { $$ = AssignmentOperator::DivideAssign; }
	| MOD_ASSIGN { $$ = AssignmentOperator::ModuloAssign; }
	| ADD_ASSIGN { $$ = AssignmentOperator::AddAssign; }
	| SUB_ASSIGN { $$ = AssignmentOperator::SubtractAssign; }
	| LEFT_ASSIGN { $$ = AssignmentOperator::LeftShiftAssign; }
	| RIGHT_ASSIGN { $$ = AssignmentOperator::RightShiftAssign; }
	| AND_ASSIGN { $$ = AssignmentOperator::BitwiseAndAssign; }
	| XOR_ASSIGN { $$ = AssignmentOperator::BitwiseXorAssign; }
	| OR_ASSIGN { $$ = AssignmentOperator::BitwiseOrAssign; }
	;

expression
	: assignment_expression { $$ = new Expression(AssignmentExpressionPtr($1)); }
	| expression ',' assignment_expression { $$ = new Expression(ExpressionPtr($1), AssignmentExpressionPtr($3)); }
	;

constant_expression
	: conditional_expression
	;

// Hmmmm
// declaration (NEEDS CREATING)
// specifiers ; (pointless) or specifiers init_declarator_list ; (BITSET?/FLAGS?) (NEEDS CREATING)
// NA                      init_declarator_list ; = init declarator (can be list) (NEEDS CREATING)
//                         declarator or declarator = initializer (NEEDS IMPL)
declaration
	: declaration_specifiers ';'
	| declaration_specifiers init_declarator_list ';'
	;

declaration_specifiers
	: storage_class_specifier
	| storage_class_specifier declaration_specifiers
	| type_specifier { $$ = $1; }
	| type_specifier declaration_specifiers
	;

init_declarator_list
	: init_declarator
	| init_declarator_list ',' init_declarator
	;

init_declarator
	: declarator { $$ = new InitDeclarator(DeclaratorPtr($1)); }
	| declarator '=' initializer { $$ = new InitDeclarator(DeclaratorPtr($1), InitializerPtr($3)); }
	;

storage_class_specifier
	: TYPEDEF
	| EXTERN { std::cerr << "The extern keyword is not supported" << std::endl; exit(1); }
	| STATIC { std::cerr << "The static keyword is not supported" << std::endl; exit(1); }
	| AUTO
	| REGISTER
	;

type_specifier
	: VOID
	| CHAR
	| SHORT { std::cerr << "Short type is unsupported." << std::endl; exit(1); }
	| INT { $$ = TypeSpecifier::INT; }
	| LONG { std::cerr << "Long type is unsupported." << std::endl; exit(1); }
	| FLOAT
	| DOUBLE
	| SIGNED
	| UNSIGNED
  | struct_specifier
	| enum_specifier
	| TYPE_NAME
	;

struct_specifier
	: STRUCT IDENTIFIER '{' struct_declaration_list '}'
	| STRUCT '{' struct_declaration_list '}'
	| STRUCT IDENTIFIER
	;

struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';'
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator_list ',' struct_declarator
	;

struct_declarator
	: declarator
	| ':' constant_expression
	| declarator ':' constant_expression
	;

enum_specifier
	: ENUM '{' enumerator_list '}'
	| ENUM IDENTIFIER '{' enumerator_list '}'
	| ENUM IDENTIFIER
	;

enumerator_list
	: enumerator
	| enumerator_list ',' enumerator
	;

enumerator
	: IDENTIFIER
	| IDENTIFIER '=' constant_expression
	;

declarator
	: pointer direct_declarator
	| direct_declarator { $$ = $1; $$->Indirect(); }
	;

// TODO this is complicated and may need more work
// Most of the recursion here is unsupported in the C std hence we don't have a list of `ParameterList`s etc.
direct_declarator
	: IDENTIFIER {
		$$ = new Declarator(*$1, true);
	}
	| '(' declarator ')' {
	    $$ = $2;
	    $$->Direct();
	}
	| direct_declarator '[' constant_expression ']'
	| direct_declarator '[' ']'
	| direct_declarator '(' parameter_list ')' {
        $$ = new FunctionDeclarator(DeclaratorPtr($1), ParameterListPtr($3));
	}
	| direct_declarator '(' identifier_list ')' {
	    std::cerr << "Need to support identifier_list in direct_declarator" << std::endl;
	    exit(1);
	}
	| direct_declarator '(' ')' {
		$$ = new FunctionDeclarator(DeclaratorPtr($1));
	}
	;

pointer
	: '*'
	| '*' pointer
	;

parameter_list
	: parameter_declaration { $$ = new ParameterList(ParameterDeclarationPtr($1)); }
	| parameter_list ',' parameter_declaration { $1->PushBack(ParameterDeclarationPtr($3)); $$=$1; }
	;

parameter_declaration
	: declaration_specifiers declarator { $$ = new ParameterDeclaration($1, DeclaratorPtr($2)); }
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;

identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER { std::cerr << "An identifier list has been created. Note that K&R style declarations are not supported." << std::endl; exit(1); }
	;

type_name
	: specifier_qualifier_list
	| specifier_qualifier_list abstract_declarator
	;

abstract_declarator
	: pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
	;

direct_abstract_declarator
	: '(' abstract_declarator ')'
	| '[' ']'
	| '[' constant_expression ']'
	| direct_abstract_declarator '[' ']'
	| direct_abstract_declarator '[' constant_expression ']'
	| '(' ')'
	| '(' parameter_list ')'
	| direct_abstract_declarator '(' ')'
	| direct_abstract_declarator '(' parameter_list ')'
	;

initializer
	: assignment_expression
	| '{' initializer_list '}'
	| '{' initializer_list ',' '}'
	;

initializer_list
	: initializer
	| initializer_list ',' initializer
	;

statement
	: labeled_statement
	| compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement { $$ = $1; }
	;

labeled_statement
	: IDENTIFIER ':' statement
	| CASE constant_expression ':' statement
	| DEFAULT ':' statement
	;

compound_statement
    // Scoping logic required here?
	: '{' '}' {
		// TODO: correct this
		std::cerr << "Need to fix issues in compound_statement" << std::endl;
		$$ = nullptr;
	}
	| '{' statement_list '}' {
		$$ = $2; // This should work for now
	}
	| '{' declaration_list '}' {
		// TODO: correct this
		$$ = nullptr;
		std::cerr << "Need to fix issues in compound_statement" << std::endl;
	}
	| '{' declaration_list statement_list '}'  {
		// TODO: correct this
		$$ = $3; // Temp - just pass the statement list
		std::cerr << "Need to fix issues in compound_statement" << std::endl;
	}
	;

declaration_list
	: declaration
	| declaration_list declaration
	;

statement_list
	: statement { $$ = new NodeList(NodePtr($1)); }
	| statement_list statement { $1->PushBack(NodePtr($2)); $$=$1; }
	;

expression_statement
	: ';'
	| expression ';' { $$ = $1; }
	;

selection_statement
	: IF '(' expression ')' statement
	| IF '(' expression ')' statement ELSE statement
	| SWITCH '(' expression ')' statement
	;

iteration_statement
	: WHILE '(' expression ')' statement
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' expression_statement expression_statement ')' statement
	| FOR '(' expression_statement expression_statement expression ')' statement
	;

jump_statement
    : GOTO IDENTIFIER ';' { std::cerr << "goto keyword is unsupported" << std::endl; exit(1); }
	| CONTINUE ';'
	| BREAK ';'
	| RETURN ';' {
		$$ = new ReturnStatement(nullptr);
	}
	| RETURN expression ';' {
		$$ = new ReturnStatement(NodePtr($2));
	}
	;



%%

Node* g_root;

NodePtr ParseAST(std::string file_name)
{
  yyin = fopen(file_name.c_str(), "r");
  if(yyin == NULL){
    std::cerr << "Couldn't open input file: " << file_name << std::endl;
    exit(1);
  }
  g_root = nullptr;
  yyparse();
  fclose(yyin);
  yylex_destroy();
  return NodePtr(g_root);
}
