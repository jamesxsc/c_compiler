// Adapted from: https://www.lysator.liu.se/c/ANSI-C-grammar-y.html

%code requires{
    #include "ast.hpp"
    #include <cassert>
	#include "ast_string_literal.hpp"
	#include "ast_continue_statement.hpp"
	#include "ast_break_statement.hpp"
	#include "ast_unary_expression.hpp"
    using namespace ast;

    extern Node* g_root;
    extern FILE* yyin;
    int yylex(void);
    void yyerror(const char*);
    int yylex_destroy(void);
}

// Represents the value associated with any kind of AST node.
%union{
  Node*               node;
  NodeList*           node_list;
  ExpressionBase*     expression_base;
  PostfixExpression*  postfix_expression;
  FunctionCallExpression* function_call_expression;
  UnaryExpression*    unary_expression; 
  UnaryOperator       unary_operator;
  MultiplicativeExpression* multiplicative_expression;
  AdditiveExpression* additive_expression;
  ShiftExpression*    shift_expression;
  RelationalExpression* relational_expression;
  EqualityExpression* equality_expression;
  AndExpression*      and_expression;
  ExclusiveOrExpression* exclusive_or_expression;
  InclusiveOrExpression* inclusive_or_expression;
  LogicalAndExpression* logical_and_expression;
  LogicalOrExpression*  logical_or_expression;
  ConditionalExpression* conditional_expression;
  AssignmentExpression* assignment_expression; 
  AssignmentOperator  assignment_operator;
  ArgumentExpressionList* argument_expression_list;
  Expression*         expression;
  ParameterDeclaration* parameter_declaration;
  Declarator*         direct_declarator;
  InitDeclarator*     init_declarator;
  InitDeclaratorList* init_declarator_list;
  Initializer*        initializer;
  ParameterList*      parameter_list;
  Declaration*        declaration;
  DeclarationList*    declaration_list;
  Statement*          statement;
  StatementList*      statement_list;
  CompoundStatement*  compound_statement;
  ExpressionStatement* expression_statement;
  int                 number_int;
  double              number_float;
  std::string*        string;
  TypeSpecifier       type_specifier;
  yytokentype         token;
}

%token IDENTIFIER INT_CONSTANT FLOAT_CONSTANT STRING_LITERAL
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP AND_OP OR_OP
%token MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token TYPE_NAME TYPEDEF EXTERN STATIC AUTO REGISTER SIZEOF
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%type <node_list> translation_unit
%type <node> external_declaration function_definition
%type <expression_base> primary_expression
%type <expression_base> constant_expression
%type <node> initializer_list
%type <node> struct_specifier struct_declaration_list struct_declaration specifier_qualifier_list struct_declarator_list
%type <node> struct_declarator enum_specifier enumerator_list enumerator pointer
%type <node> identifier_list type_name abstract_declarator direct_abstract_declarator labeled_statement
%type <node> jump_statement

%nterm <statement> statement
%nterm <statement_list> statement_list
%nterm <compound_statement> compound_statement
%nterm <expression_statement> expression_statement
%nterm <statement> selection_statement iteration_statement

%nterm <declaration_list> declaration_list
%nterm <parameter_list> parameter_list

%nterm <direct_declarator> direct_declarator declarator
%nterm <init_declarator> init_declarator
%nterm <init_declarator_list> init_declarator_list
%nterm <initializer> initializer
%nterm <declaration> declaration
%nterm <parameter_declaration> parameter_declaration

%nterm <postfix_expression> postfix_expression
%nterm <function_call_expression> function_call_expression
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
%nterm <argument_expression_list> argument_expression_list
%nterm <expression> expression

%type <string> storage_class_specifier
%type <number_int> INT_CONSTANT STRING_LITERAL
%type <number_float> FLOAT_CONSTANT
%type <string> IDENTIFIER
%type <type_specifier> type_specifier
%type <type_specifier> declaration_specifiers

%start ROOT
%%

ROOT
    : translation_unit { g_root = $1; }
    ;

translation_unit
    : external_declaration { $$ = new NodeList(NodePtr($1)); }
    | translation_unit external_declaration { $1->PushBack(NodePtr($2)); $$ = $1; }
    ;

external_declaration
    : function_definition { $$ = $1; }
    | declaration { $$ = $1; }
    ;

function_definition
    : declaration_specifiers declarator declaration_list compound_statement
    | declaration_specifiers declarator compound_statement {
        auto * decl = dynamic_cast<FunctionDeclarator*>($2);
        assert(decl != nullptr && "Expected a function declarator in function_definition");
        $3->SetFunction();
        $$ = new FunctionDefinition($1, FunctionDeclaratorPtr(decl), CompoundStatementPtr($3));
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
    | FLOAT_CONSTANT {
        $$ = new FloatConstant($1);
    }
    | STRING_LITERAL {
        $$ = new StringLiteral(*$1);
    }
    | '(' expression ')' {
        $$ = $2;
    }
    ;

function_call_expression
    : postfix_expression '(' argument_expression_list ')' {
        $$ = new FunctionCallExpression(PostfixExpressionPtr($1), ArgumentExpressionListPtr($3));
    }
    | postfix_expression '(' ')' {
        $$ = new FunctionCallExpression(PostfixExpressionPtr($1));
    }
    ;

postfix_expression
    : primary_expression { $$ = new PostfixExpression(ExpressionBasePtr($1)); }
    | postfix_expression '[' expression ']' { /* Array subscripting (not fully implemented) */ }
    | function_call_expression { $$ = new PostfixExpression(ExpressionBasePtr($1)); }
    | postfix_expression '.' IDENTIFIER { /* Struct member access (not fully implemented) */ }
    | postfix_expression PTR_OP IDENTIFIER { /* Struct pointer access (not fully implemented) */ }
    | postfix_expression INC_OP { /* Post-increment (not fully implemented) */ }
    | postfix_expression DEC_OP { /* Post-decrement (not fully implemented) */ }
    ;

argument_expression_list
    : assignment_expression { $$ = new ArgumentExpressionList(AssignmentExpressionPtr($1)); }
    | argument_expression_list ',' assignment_expression {
        $1->PushBack(AssignmentExpressionPtr($3));
        $$=$1;
    }
    ;

unary_expression
    : postfix_expression { $$ = new UnaryExpression(PostfixExpressionPtr($1)); }
    | INC_OP unary_expression { /* Pre-increment (not fully implemented) */ }
    | DEC_OP unary_expression { /* Pre-decrement (not fully implemented) */ }
    | unary_operator multiplicative_expression {
        $$ = new UnaryExpression(MultiplicativeExpressionPtr($2), $1);
    }
    | SIZEOF unary_expression { /* sizeof expression (not fully implemented) */ }
    | SIZEOF '(' type_name ')' { /* sizeof type (not fully implemented) */ }
    ;

unary_operator
    : '&' { $$ = UnaryOperator::AddressOf; }
    | '*' { $$ = UnaryOperator::Dereference; }
    | '+' { $$ = UnaryOperator::Plus; }
    | '-' { $$ = UnaryOperator::Minus; }
    | '~' { $$ = UnaryOperator::BitwiseNot; }
    | '!' { $$ = UnaryOperator::LogicalNot; }
    ;

multiplicative_expression
    : unary_expression {
        $$ = new MultiplicativeExpression(UnaryExpressionPtr($1));
    }
    | multiplicative_expression '*' unary_expression {
        $$ = new MultiplicativeExpression(MultiplicativeExpressionPtr($1), UnaryExpressionPtr($3), MultiplicativeOperator::Multiply);
    }
    | multiplicative_expression '/' unary_expression {
        $$ = new MultiplicativeExpression(MultiplicativeExpressionPtr($1), UnaryExpressionPtr($3), MultiplicativeOperator::Divide);
    }
    | multiplicative_expression '%' unary_expression {
        $$ = new MultiplicativeExpression(MultiplicativeExpressionPtr($1), UnaryExpressionPtr($3), MultiplicativeOperator::Modulo);
    }
    ;

additive_expression
    : multiplicative_expression {
        $$ = new AdditiveExpression(MultiplicativeExpressionPtr($1));
    }
    | additive_expression '+' multiplicative_expression {
        $$ = new AdditiveExpression(AdditiveExpressionPtr($1), MultiplicativeExpressionPtr($3), AdditiveOperator::Add);
    }
    | additive_expression '-' multiplicative_expression {
        $$ = new AdditiveExpression(AdditiveExpressionPtr($1), MultiplicativeExpressionPtr($3), AdditiveOperator::Subtract);
    }
    ;

shift_expression
    : additive_expression {
        $$ = new ShiftExpression(AdditiveExpressionPtr($1));
    }
    | shift_expression LEFT_OP additive_expression {
        $$ = new ShiftExpression(ShiftExpressionPtr($1), AdditiveExpressionPtr($3), ShiftOperator::Left);
    }
    | shift_expression RIGHT_OP additive_expression {
        $$ = new ShiftExpression(ShiftExpressionPtr($1), AdditiveExpressionPtr($3), ShiftOperator::Right);
    }
    ;

relational_expression
    : shift_expression {
        $$ = new RelationalExpression(ShiftExpressionPtr($1));
    }
    | relational_expression '<' shift_expression {
        $$ = new RelationalExpression(RelationalExpressionPtr($1), ShiftExpressionPtr($3), RelationalOperator::LessThan);
    }
    | relational_expression '>' shift_expression {
        $$ = new RelationalExpression(RelationalExpressionPtr($1), ShiftExpressionPtr($3), RelationalOperator::GreaterThan);
    }
    | relational_expression LE_OP shift_expression {
        $$ = new RelationalExpression(RelationalExpressionPtr($1), ShiftExpressionPtr($3), RelationalOperator::LessThanOrEqual);
    }
    | relational_expression GE_OP shift_expression {
        $$ = new RelationalExpression(RelationalExpressionPtr($1), ShiftExpressionPtr($3), RelationalOperator::GreaterThanOrEqual);
    }
    ;

equality_expression
    : relational_expression {
        $$ = new EqualityExpression(RelationalExpressionPtr($1));
    }
    | equality_expression EQ_OP relational_expression {
        $$ = new EqualityExpression(EqualityExpressionPtr($1), RelationalExpressionPtr($3), EqualityOperator::Equality);
    }
    | equality_expression NE_OP relational_expression {
        $$ = new EqualityExpression(EqualityExpressionPtr($1), RelationalExpressionPtr($3), EqualityOperator::Inequality);
    }
    ;

and_expression
    : equality_expression {
        $$ = new AndExpression(EqualityExpressionPtr($1));
    }
    | and_expression '&' equality_expression {
        $$ = new AndExpression(AndExpressionPtr($1), EqualityExpressionPtr($3));
    }
    ;

exclusive_or_expression
    : and_expression {
        $$ = new ExclusiveOrExpression(AndExpressionPtr($1));
    }
    | exclusive_or_expression '^' and_expression {
        $$ = new ExclusiveOrExpression(ExclusiveOrExpressionPtr($1), AndExpressionPtr($3));
    }
    ;

inclusive_or_expression
    : exclusive_or_expression {
        $$ = new InclusiveOrExpression(ExclusiveOrExpressionPtr($1));
    }
    | inclusive_or_expression '|' exclusive_or_expression {
        $$ = new InclusiveOrExpression(InclusiveOrExpressionPtr($1), ExclusiveOrExpressionPtr($3));
    }
    ;

logical_and_expression
    : inclusive_or_expression {
        $$ = new LogicalAndExpression(InclusiveOrExpressionPtr($1));
    }
    | logical_and_expression AND_OP inclusive_or_expression {
        $$ = new LogicalAndExpression(LogicalAndExpressionPtr($1), InclusiveOrExpressionPtr($3));
    }
    ;

logical_or_expression
    : logical_and_expression {
        $$ = new LogicalOrExpression(LogicalAndExpressionPtr($1));
    }
    | logical_or_expression OR_OP logical_and_expression {
        $$ = new LogicalOrExpression(LogicalOrExpressionPtr($1), LogicalAndExpressionPtr($3));
    }
    ;

conditional_expression
    : logical_or_expression {
        $$ = new ConditionalExpression(LogicalOrExpressionPtr($1));
    }
    | logical_or_expression '?' expression ':' conditional_expression {
        $$ = new ConditionalExpression(LogicalOrExpressionPtr($1), ExpressionPtr($3), ConditionalExpressionPtr($5));
    }
    ;

assignment_expression
    : conditional_expression {
        $$ = new AssignmentExpression(ConditionalExpressionPtr($1));
    }
    | unary_expression assignment_operator assignment_expression {
        $$ = new AssignmentExpression(UnaryExpressionPtr($1), $2, AssignmentExpressionPtr($3));
    }
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
    : assignment_expression {
        $$ = new Expression(AssignmentExpressionPtr($1));
    }
    | expression ',' assignment_expression {
        $$ = new Expression(ExpressionPtr($1), AssignmentExpressionPtr($3));
    }
    ;

constant_expression
    : conditional_expression
    ;

declaration
    : declaration_specifiers ';' {
        std::cerr << "Need to implement declaration specifiers only declaration" << std::endl;
        exit(1);
    }
    | declaration_specifiers init_declarator_list ';' {
        $$ = new Declaration($1, InitDeclaratorListPtr($2));
    }
    ;

declaration_specifiers
    : storage_class_specifier
    | storage_class_specifier declaration_specifiers
    | type_specifier { $$ = $1; }
    | type_specifier declaration_specifiers
    ;

init_declarator_list
    : init_declarator {
        $$ = new InitDeclaratorList(InitDeclaratorPtr($1));
    }
    | init_declarator_list ',' init_declarator {
        $1->PushBack(InitDeclaratorPtr($3));
        $$=$1;
    }
    ;

init_declarator
    : declarator {
        $$ = new InitDeclarator(DeclaratorPtr($1));
    }
    | declarator '=' initializer {
        $$ = new InitDeclarator(DeclaratorPtr($1), InitializerPtr($3));
    }
    ;

storage_class_specifier
    : TYPEDEF
    | EXTERN {
        std::cerr << "The extern keyword is not supported" << std::endl;
        exit(1);
    }
    | STATIC {
        std::cerr << "The static keyword is not supported" << std::endl;
        exit(1);
    }
    | AUTO
    | REGISTER
    ;

type_specifier
    : VOID {
        std::cerr << "Void type is partially supported." << std::endl;
        exit(1);
    }
    | CHAR {
        std::cerr << "Char type is unsupported." << std::endl;
        exit(1);
    }
    | SHORT {
        std::cerr << "Short type is unsupported." << std::endl;
        exit(1);
    }
    | INT {
        $$ = TypeSpecifier::INT;
    }
    | LONG {
        std::cerr << "Long type is unsupported." << std::endl;
        exit(1);
    }
    | FLOAT {
        $$ = TypeSpecifier::FLOAT;
    }
    | DOUBLE {
        $$ = TypeSpecifier::DOUBLE;
    }
    | SIGNED {
        std::cerr << "Signed type is unsupported." << std::endl;
        exit(1);
    }
    | UNSIGNED {
        std::cerr << "Unsigned type is unsupported." << std::endl;
        exit(1);
    }
    | struct_specifier {
        std::cerr << "Structs are not fully implemented." << std::endl;
        exit(1);
    }
    | enum_specifier {
        std::cerr << "Enums are not fully implemented." << std::endl;
        exit(1);
    }
    | TYPE_NAME {
        std::cerr << "TYPE_NAME is not fully supported." << std::endl;
        exit(1);
    }
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
    | direct_declarator {
        $$ = $1;
        $$->Indirect();
    }
    ;

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
    : parameter_declaration {
        $$ = new ParameterList(ParameterDeclarationPtr($1));
    }
    | parameter_list ',' parameter_declaration {
        $1->PushBack(ParameterDeclarationPtr($3));
        $$=$1;
    }
    ;

parameter_declaration
    : declaration_specifiers declarator {
        $$ = new ParameterDeclaration($1, DeclaratorPtr($2));
    }
    | declaration_specifiers abstract_declarator {
        std::cerr << "abstract_declarator in parameter_declaration is not fully supported" << std::endl;
        exit(1);
    }
    | declaration_specifiers {
        std::cerr << "parameter_declaration with no declarator is not fully supported" << std::endl;
        exit(1);
    }
    ;

identifier_list
    : IDENTIFIER {
        std::cerr << "An identifier list has been created. Note that K&R style declarations are not supported." << std::endl;
        exit(1);
    }
    | identifier_list ',' IDENTIFIER {
        std::cerr << "An identifier list has been created. Note that K&R style declarations are not supported." << std::endl;
        exit(1);
    }
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
    : assignment_expression {
        $$ = new Initializer(AssignmentExpressionPtr($1));
    }
    | '{' initializer_list '}' {
        std::cerr << "Array/struct initializer is not fully implemented." << std::endl;
        exit(1);
    }
    | '{' initializer_list ',' '}' {
        std::cerr << "Array/struct initializer with trailing comma is not fully implemented." << std::endl;
        exit(1);
    }
    ;

initializer_list
    : initializer {
        $$ = $1;
    }
    | initializer_list ',' initializer {
        std::cerr << "Multiple initializers are not fully implemented." << std::endl;
        exit(1);
    }
    ;

statement
    : labeled_statement
    | compound_statement { $$ = $1; }
    | expression_statement { $$ = $1; }
    | selection_statement { $$ = $1; }
    | iteration_statement { $$ = $1; }
    | jump_statement
    ;

labeled_statement
    : IDENTIFIER ':' statement
    | CASE constant_expression ':' statement
    | DEFAULT ':' statement
    ;

compound_statement
    : '{' '}' {
        $$ = new CompoundStatement(nullptr, nullptr);
    }
    | '{' statement_list '}' {
        $$ = new CompoundStatement(nullptr, StatementListPtr($2));
    }
    | '{' declaration_list '}' {
        $$ = new CompoundStatement(DeclarationListPtr($2), nullptr);
    }
    | '{' declaration_list statement_list '}' {
        $$ = new CompoundStatement(DeclarationListPtr($2), StatementListPtr($3));
    }
    ;

declaration_list
    : declaration {
        $$ = new DeclarationList(DeclarationPtr($1));
    }
    | declaration_list declaration {
        $1->PushBack(DeclarationPtr($2));
        $$ = $1;
    }
    ;

statement_list
    : statement {
        $$ = new StatementList(StatementPtr($1));
    }
    | statement_list statement {
        $1->PushBack(StatementPtr($2));
        $$=$1;
    }
    ;

expression_statement
    : ';' {
        $$ = new ExpressionStatement();
    }
    | expression ';' {
        $$ = new ExpressionStatement(ExpressionPtr($1));
    }
    ;

selection_statement
  : IF '(' expression ')' statement {
      $$ = new IfStatement(ExpressionPtr($3), StatementPtr($5), nullptr);
    }
  | IF '(' expression ')' statement ELSE statement {
      $$ = new IfStatement(ExpressionPtr($3), StatementPtr($5), StatementPtr($7));
    }
  | SWITCH '(' expression ')' statement {
      $$ = new SwitchStatement(ExpressionPtr($3), StatementPtr($5));
    }
  ;

iteration_statement
  : WHILE '(' expression ')' statement {
      $$ = new WhileStatement(ExpressionPtr($3), StatementPtr($5));
    }
  | DO statement WHILE '(' expression ')' ';' {
      $$ = new DoWhileStatement(StatementPtr($2), ExpressionPtr($5));
    }
  | FOR '(' expression_statement expression_statement ')' statement {
      $$ = new ForStatement(
        ExpressionStatementPtr($3),
        ExpressionStatementPtr($4),
        nullptr,
        StatementPtr($6)
      );
    }
  | FOR '(' expression_statement expression_statement expression ')' statement {
      $$ = new ForStatement(
        ExpressionStatementPtr($3),
        ExpressionStatementPtr($4),
        ExpressionPtr($5),
        StatementPtr($7)
      );
    }
  ;

jump_statement
    : GOTO IDENTIFIER ';' {
        std::cerr << "goto keyword is unsupported" << std::endl;
        exit(1);
    }
    | CONTINUE ';' {
        $$ = new ContinueStatement();
    }
    | BREAK ';' {
        $$ = new BreakStatement();
    }
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
