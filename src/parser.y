// Adapted from: https://www.lysator.liu.se/c/ANSI-C-grammar-y.html

%code requires{
    #include "ast.hpp"
    #include <cassert>
    #include <unordered_map>

    using namespace ast;

    extern int yylineno;
    extern char* yytext;
    extern Node* g_root;
    extern std::unordered_map<std::string, TypeSpecifier> typedefs;
    extern FILE* yyin;
    int yylex(void);
    void yyerror(const char*);
    int yylex_destroy(void);
}
    // todo make scoped if we have time (typedefs)

// Represents the value associated with any kind of AST node.
%union{
  Node*        	node;
  NodeList*    	node_list;
  ExpressionBase* expression_base;
  PostfixExpression* postfix_expression;
  FunctionCallExpression* function_call_expression;
  StructMemberAccessExpression* struct_member_access_expression;
  ArrayIndexExpression* array_index_expression;
  UnaryExpression* unary_expression; 
  UnaryOperator unary_operator;
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
  ConstantExpression* constant_expression;
  AssignmentExpression* assignment_expression; 
  AssignmentOperator assignment_operator;
  ArgumentExpressionList* argument_expression_list;
  Expression*  	expression;
  ParameterDeclaration*     parameter_declaration;
  Declarator*         direct_declarator;
  InitDeclarator*     init_declarator;
  InitDeclaratorList* init_declarator_list;
  Initializer*        initializer;
  ParameterList* parameter_list;
  DeclarationSpecifiers* declaration_specifiers;
  StorageClassSpecifier storage_class_specifier;
  Declaration* declaration;
  DeclarationList* declaration_list;
  Statement* statement;
  StatementList* statement_list;
  CompoundStatement* compound_statement;
  ExpressionStatement* expression_statement;
  LabeledStatement* labeled_statement;
  JumpStatement* jump_statement;
  SpecifierQualifierList* specifier_qualifier_list;
  TypeName* type_name;
  Enumerator* enumerator;
  EnumeratorList* enumerator_list;
  EnumSpecifier* enum_specifier;
  StructDeclarator* struct_declarator;
  StructDeclaratorList* struct_declarator_list;
  StructDeclaration* struct_declaration;
  StructDeclarationList* struct_declaration_list;
  StructSpecifier* struct_specifier;
  int          	number_int;
  double       	number_float;
  std::string* 	string;
  TypeSpecifier*   type_specifier;
  yytokentype  	token;
}

%token IDENTIFIER INT_CONSTANT FLOAT_CONSTANT DOUBLE_CONSTANT CHAR_CONSTANT STRING_LITERAL
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP AND_OP OR_OP
%token MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%token TYPE_NAME TYPEDEF EXTERN STATIC AUTO REGISTER SIZEOF
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

// Note: Prefer %nterm over %type

// Unchanged types
%type <node_list> translation_unit
%type <node> external_declaration function_definition
%type <node> identifier_list abstract_declarator direct_abstract_declarator

// Statement types
%nterm <statement> statement
%nterm <labeled_statement> labeled_statement
%nterm <statement_list> statement_list
%nterm <compound_statement> compound_statement
%nterm <expression_statement> expression_statement
%nterm <jump_statement> jump_statement
%nterm <statement> selection_statement iteration_statement

%nterm <declaration_list> declaration_list
%nterm <parameter_list> parameter_list

%nterm <direct_declarator> direct_declarator declarator // Differentiated by a bool member
%nterm <init_declarator> init_declarator
%nterm <init_declarator_list> init_declarator_list
%nterm <initializer> initializer initializer_list
%nterm <declaration> declaration
%nterm <declaration_specifiers> declaration_specifiers
%nterm <storage_class_specifier> storage_class_specifier
%nterm <parameter_declaration> parameter_declaration

// Expression hierarchy
%type <expression_base> primary_expression
%nterm <postfix_expression> postfix_expression
%nterm <function_call_expression> function_call_expression
%nterm <struct_member_access_expression> struct_member_access_expression
%nterm <array_index_expression> array_index_expression
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
%nterm <constant_expression> constant_expression
%nterm <assignment_expression> assignment_expression
%nterm <assignment_operator> assignment_operator
%nterm <argument_expression_list> argument_expression_list
%nterm <expression> expression

%nterm <specifier_qualifier_list> specifier_qualifier_list
%nterm <type_name> type_name

// Enums
%nterm <enumerator> enumerator
%nterm <enumerator_list> enumerator_list
%nterm <enum_specifier> enum_specifier

// Structs
%nterm <struct_declarator> struct_declarator
%nterm <struct_declarator_list> struct_declarator_list
%nterm <struct_declaration> struct_declaration
%nterm <struct_declaration_list> struct_declaration_list
%nterm <struct_specifier> struct_specifier

%type <number_int> INT_CONSTANT CHAR_CONSTANT
%type <number_float> FLOAT_CONSTANT DOUBLE_CONSTANT
%type <string> IDENTIFIER TYPE_NAME STRING_LITERAL
%type <type_specifier> type_specifier

%type <number_int> pointer


%start ROOT
%%

ROOT
    : translation_unit { g_root = $1; }
    ;


translation_unit
// Use nodelist for now
	: external_declaration { $$ = new NodeList(NodePtr($1)); }
	| translation_unit external_declaration { $1->PushBack(NodePtr($2)); $$ = $1; }
	;

external_declaration
	: function_definition { $$ = $1; }
	| declaration {
	    AggregateTypeDefinition* agg = dynamic_cast<AggregateTypeDefinition*>($1);
	    if (agg != nullptr) { agg->SetGlobal(); $$ = agg; } else $$ = new ExternalDeclaration(DeclarationPtr($1));
	}
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement
	| declaration_specifiers declarator compound_statement {
	    // Check we didn't somehow get a non-function declarator
	    auto * decl = dynamic_cast<FunctionDeclarator*>($2);
	    assert(decl != nullptr && "Expected a function declarator in function_definition");
	    $3->SetFunction();
		$$ = new FunctionDefinition(DeclarationSpecifiersPtr($1), FunctionDeclaratorPtr(decl), CompoundStatementPtr($3));
	}
	| declarator declaration_list compound_statement
	| declarator compound_statement {
	    std::cerr << "Unimplemented function definition grammar" << std::endl;
	}
	;


primary_expression
	: IDENTIFIER { $$ = new Identifier(*$1); delete $1; }
	| INT_CONSTANT { $$ = new IntConstant($1); }
	| CHAR_CONSTANT { $$ = new CharConstant($1); }
    | FLOAT_CONSTANT { $$ = new FloatConstant($1, false); }
    | DOUBLE_CONSTANT { $$ = new FloatConstant($1, true); }
	| STRING_LITERAL { $$ = new StringConstant(*$1); delete $1; }
	| '(' expression ')' { $$ = new ParenthesisedExpression(ExpressionPtr($2)); }
	;

// Function call/array; the extra class here seems the neatest way to do this rather than bloat PostfixExpression
function_call_expression
    : postfix_expression '(' argument_expression_list ')' { $$ = new FunctionCallExpression(PostfixExpressionPtr($1), ArgumentExpressionListPtr($3)); }
	| postfix_expression '(' ')' { $$ = new FunctionCallExpression(PostfixExpressionPtr($1)); }
    ;

array_index_expression
    : postfix_expression '[' expression ']' { $$ = new ArrayIndexExpression(PostfixExpressionPtr($1), ExpressionPtr($3)); }
    ;

struct_member_access_expression
    : postfix_expression '.' IDENTIFIER { $$ = new StructMemberAccessExpression(PostfixExpressionPtr($1), *$3); delete $3; }
    | postfix_expression PTR_OP IDENTIFIER { $$ = new StructMemberAccessExpression(PostfixExpressionPtr($1), *$3, true); delete $3; }
    ;

postfix_expression
	: primary_expression { $$ = new PostfixExpression(ExpressionBasePtr($1), PostfixOperator::PrimaryPromote); }
    | array_index_expression { $$ = new PostfixExpression(ExpressionBasePtr($1), PostfixOperator::ArrayIndexPromote); }
	| function_call_expression { $$ = new PostfixExpression(ExpressionBasePtr($1), PostfixOperator::FunctionCallPromote); }
	| struct_member_access_expression { $$ = new PostfixExpression(ExpressionBasePtr($1), PostfixOperator::StructMemberAccessPromote); }
	| postfix_expression INC_OP { $$ = new PostfixExpression(PostfixExpressionPtr($1), PostfixOperator::PostfixIncrement); }
	| postfix_expression DEC_OP { $$ = new PostfixExpression(PostfixExpressionPtr($1), PostfixOperator::PostfixDecrement); }
	;

argument_expression_list
	: assignment_expression { $$ = new ArgumentExpressionList(AssignmentExpressionPtr($1)); }
	| argument_expression_list ',' assignment_expression { $1->PushBack(AssignmentExpressionPtr($3)); $$=$1; }
	;

unary_expression
	: postfix_expression { $$ = new UnaryExpression(PostfixExpressionPtr($1)); }
	| INC_OP unary_expression { $$ = new UnaryExpression(UnaryExpressionPtr($2), UnaryOperator::PrefixIncrement); }
	| DEC_OP unary_expression { $$ = new UnaryExpression(UnaryExpressionPtr($2), UnaryOperator::PrefixDecrement); }
//	| unary_operator cast_expression // Casts are not required to be implemented
	| unary_operator multiplicative_expression { $$ = new UnaryExpression(MultiplicativeExpressionPtr($2), $1); }
	| SIZEOF unary_expression { $$ = new UnaryExpression(UnaryExpressionPtr($2), UnaryOperator::SizeofUnary); }
	| SIZEOF '(' type_name ')' { $$ = new UnaryExpression(TypeNamePtr($3)); }
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
	: conditional_expression { $$ = new ConstantExpression(ConditionalExpressionPtr($1)); }
	;

declaration
	: declaration_specifiers ';' // We modified the grammar so don't use this, allows a different type for usage as type specifier and definition
	| enum_specifier ';' { $$ = new AggregateTypeDefinition(EnumSpecifierPtr($1)); }
	| struct_specifier ';' { $$ = new AggregateTypeDefinition(StructSpecifierPtr($1)); }
	| declaration_specifiers init_declarator_list ';' {
	    $$ = new Declaration(DeclarationSpecifiersPtr($1), InitDeclaratorListPtr($2));
	    Context dummy; // Not required
	    if ($$->IsTypedef()) { for (auto & decl : *$2) {
	        TypeSpecifier type = $1->GetType(dummy);
	        if (decl->IsPointer()) type = { TypeSpecifier::POINTER, type };
	        if (decl->IsArray()) type = decl->BuildArray(type, dummy).type;
            typedefs.emplace(decl->GetIdentifier(), type);
        } } // Support typedef int x, y; syntax
    }
	;

declaration_specifiers
	: storage_class_specifier { $$ = new DeclarationSpecifiers($1); }
	| storage_class_specifier declaration_specifiers { $2->SetStorageClassSpecifier($1); $$ = $2; }
	| type_specifier { $$ = new DeclarationSpecifiers(TypeSpecifierPtr($1)); }
	| type_specifier declaration_specifiers { $2->AddTypeSpecifier(TypeSpecifierPtr($1)); $$ = $2; }
	;

init_declarator_list
	: init_declarator { $$ = new InitDeclaratorList(InitDeclaratorPtr($1)); }
	| init_declarator_list ',' init_declarator { $1->PushBack(InitDeclaratorPtr($3)); $$=$1; }
	;

init_declarator
	: declarator { $$ = new InitDeclarator(DeclaratorPtr($1)); }
	| declarator '=' initializer { $$ = new InitDeclarator(DeclaratorPtr($1), InitializerPtr($3)); }
	;

storage_class_specifier
	: TYPEDEF { $$ = StorageClassSpecifier::Typedef; }
	| EXTERN { std::cerr << "The extern keyword is not supported" << std::endl; exit(1); }
	| STATIC { std::cerr << "The static keyword is not supported" << std::endl; exit(1); }
	| AUTO { $$ = StorageClassSpecifier::Auto; }
	| REGISTER { $$ = StorageClassSpecifier::Register; }
	;

type_specifier
	: VOID { $$ = new TypeSpecifier(TypeSpecifier::VOID); }
	| CHAR { $$ = new TypeSpecifier(TypeSpecifier::CHAR); }
	| SHORT { std::cerr << "Short type is unsupported." << std::endl; exit(1); }
	| INT { $$ = new TypeSpecifier(TypeSpecifier::INT); }
	| LONG { std::cerr << "Long type is unsupported." << std::endl; exit(1); }
	| FLOAT { $$ = new TypeSpecifier(TypeSpecifier::FLOAT); }
	| DOUBLE { $$ = new TypeSpecifier(TypeSpecifier::DOUBLE); }
	| SIGNED { $$ = new TypeSpecifier(TypeSpecifier::INT); }
	| UNSIGNED { $$ = new TypeSpecifier(TypeSpecifier::UNSIGNED); }
    | struct_specifier { $$ = new TypeSpecifier($1->GetIdentifier(), true); }
	| enum_specifier { $$ = new TypeSpecifier($1->GetIdentifier(), false); }
	| TYPE_NAME { $$ = new TypeSpecifier(typedefs.at(*$1)); delete $1; }
	;

struct_specifier
	: STRUCT IDENTIFIER '{' struct_declaration_list '}' { $$ = new StructSpecifier(*$2, StructDeclarationListPtr($4)); delete $2; }
	| STRUCT '{' struct_declaration_list '}' { $$ = new StructSpecifier(StructDeclarationListPtr($3)); }
	| STRUCT IDENTIFIER { $$ = new StructSpecifier(*$2); delete $2; }
	;

struct_declaration_list
	: struct_declaration { $$ = new StructDeclarationList(StructDeclarationPtr($1)); }
	| struct_declaration_list struct_declaration { $1->PushBack(StructDeclarationPtr($2)); $$=$1; }
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';' { $$ = new StructDeclaration(SpecifierQualifierListPtr($1), StructDeclaratorListPtr($2)); }
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list { $$ = $2; $2->AddTypeSpecifier(TypeSpecifierPtr($1)); }
	| type_specifier { $$ = new SpecifierQualifierList(TypeSpecifierPtr($1)); }
	;

struct_declarator_list
	: struct_declarator { $$ = new StructDeclaratorList(StructDeclaratorPtr($1)); }
	| struct_declarator_list ',' struct_declarator { $1->PushBack(StructDeclaratorPtr($3)); $$=$1; }
	;

struct_declarator
	: declarator { $$ = new StructDeclarator(DeclaratorPtr($1)); }
	| ':' constant_expression { throw std::runtime_error("Struct bitfields are not supported."); }
	| declarator ':' constant_expression { throw std::runtime_error("Struct bitfields are not supported."); }
	;

enum_specifier
	: ENUM '{' enumerator_list '}' { $$ = new EnumSpecifier(EnumeratorListPtr($3)); }
	| ENUM IDENTIFIER '{' enumerator_list '}' { $$ = new EnumSpecifier(*$2, EnumeratorListPtr($4)); delete $2; }
	| ENUM IDENTIFIER { $$ = new EnumSpecifier(*$2); delete $2; }
	;

enumerator_list
	: enumerator { $$ = new EnumeratorList(EnumeratorPtr($1)); }
	| enumerator_list ',' enumerator { $1->PushBack(EnumeratorPtr($3)); $$=$1; }
	;

enumerator
	: IDENTIFIER { $$ = new Enumerator(*$1); delete $1; }
	| IDENTIFIER '=' constant_expression { $$ = new Enumerator(*$1, ConstantExpressionPtr($3)); delete $1; }
	;

declarator
    // todo do we need to support double pointers?
	: pointer direct_declarator {
	    // Function returning pointer isn't a pointer declarator
	    if ($2->IsFunction()) { $2->SetPointerReturn($1); $$ = $2; }
        else { $2->SetPointer($1); $$ = $2; }
	}
	| direct_declarator { $$ = $1; $$->Indirect(); }
	;

// Most of the recursion here is unsupported in the C std hence we don't have a list of `ParameterList`s etc.
direct_declarator
	: IDENTIFIER {
		$$ = new Declarator(*$1, true);
		delete $1;
	}
	| '(' declarator ')' { $$ = $2; $$->Direct(); }
	| direct_declarator '[' constant_expression ']' { $$ = new ArrayDeclarator(DeclaratorPtr($1), ConstantExpressionPtr($3)); }
	| direct_declarator '[' ']' { $1->SetPointer(1); $$ = $1; }
	| direct_declarator '(' parameter_list ')' { $$ = new FunctionDeclarator(DeclaratorPtr($1), ParameterListPtr($3)); }
	| direct_declarator '(' identifier_list ')' {
	    std::cerr << "Need to support identifier_list in direct_declarator" << std::endl;
	    exit(1);
	}
	| direct_declarator '(' ')' { $$ = new FunctionDeclarator(DeclaratorPtr($1)); }
	;

pointer
	: '*' { $$ = 1; }
	| '*' pointer { $$ = $2 + 1; }
	;

parameter_list
	: parameter_declaration { $$ = new ParameterList(ParameterDeclarationPtr($1)); }
	| parameter_list ',' parameter_declaration { $1->PushBack(ParameterDeclarationPtr($3)); $$=$1; }
	;

parameter_declaration
	: declaration_specifiers declarator { $$ = new ParameterDeclaration(DeclarationSpecifiersPtr($1), DeclaratorPtr($2)); }
	| declaration_specifiers abstract_declarator // todo would be kind of nice to have this for unused * [] etc. params
	| declaration_specifiers { $$ = new ParameterDeclaration(DeclarationSpecifiersPtr($1)); }
	;

identifier_list
	: IDENTIFIER { std::cerr << "An identifier list has been created. Note that K&R style declarations are not supported." << std::endl; exit(1); }
	| identifier_list ',' IDENTIFIER { std::cerr << "An identifier list has been created. Note that K&R style declarations are not supported." << std::endl; exit(1); }
	;

type_name
	: specifier_qualifier_list { $$ = new TypeName(SpecifierQualifierListPtr($1)); }
	| specifier_qualifier_list abstract_declarator { std::cerr << "Abstract declarators need to be implemented (for type names)." << std::endl; exit(1); }
	;

// Don't think we need this
abstract_declarator
	: pointer
//	| direct_abstract_declarator
//	| pointer direct_abstract_declarator
	;
//
direct_abstract_declarator
	: '(' abstract_declarator ')'
//	| '[' ']'
//	| '[' constant_expression ']'
//	| direct_abstract_declarator '[' ']'
//	| direct_abstract_declarator '[' constant_expression ']'
//	| '(' ')'
//	| '(' parameter_list ')'
//	| direct_abstract_declarator '(' ')'
//	| direct_abstract_declarator '(' parameter_list ')'
	;

initializer
	: assignment_expression { $$ = new Initializer(AssignmentExpressionPtr($1)); }
	| '{' initializer_list '}' { $$ = $2; }
	| '{' initializer_list ',' '}' { $$ = $2; }
	;

initializer_list
	: initializer { $$ = new InitializerList(InitializerPtr($1)); }
	| initializer_list ',' initializer { $1->AddInitializer(InitializerPtr($3)); $$ = $1; }
	;

// Do these need to be cast to StatementPtrs or are derived ptrs ok?
statement
	: labeled_statement { $$ = $1; }
	| compound_statement { $$ = $1; }
	| expression_statement { $$ = $1; }
	| selection_statement { $$ = $1; }
	| iteration_statement { $$ = $1; }
	| jump_statement { $$ = $1; }
	;

labeled_statement
	: IDENTIFIER ':' statement { $$ = new LabeledStatement(StatementPtr($3), *$1); delete $1; }
	| CASE constant_expression ':' statement { $$ = new LabeledStatement(StatementPtr($4), ConstantExpressionPtr($2)); }
	| DEFAULT ':' statement  { $$ = new LabeledStatement(StatementPtr($3)); }
	;

// This looks counterintuitive but in C90 declarations must all be at the start of a block
compound_statement
	: '{' '}' { $$ = new CompoundStatement(nullptr, nullptr); }
	| '{' statement_list '}' { $$ = new CompoundStatement(nullptr, StatementListPtr($2)); }
	| '{' declaration_list '}' { $$ = new CompoundStatement(DeclarationListPtr($2), nullptr); }
	| '{' declaration_list statement_list '}'  { $$ = new CompoundStatement(DeclarationListPtr($2), StatementListPtr($3)); }
	;

declaration_list
	: declaration { $$ = new DeclarationList(DeclarationPtr($1)); }
	| declaration_list declaration { $1->PushBack(DeclarationPtr($2)); $$ = $1; }
	;

statement_list
	: statement { $$ = new StatementList(StatementPtr($1)); }
	| statement_list statement { $1->PushBack(StatementPtr($2)); $$=$1; }
	;

expression_statement
	: ';' { $$ = new ExpressionStatement(); }
	| expression ';' { $$ = new ExpressionStatement(ExpressionPtr($1)); }
	;

selection_statement
    : IF '(' expression ')' statement { $$ = new IfStatement(ExpressionPtr($3), StatementPtr($5), nullptr); }
    | IF '(' expression ')' statement ELSE statement { $$ = new IfStatement(ExpressionPtr($3), StatementPtr($5), StatementPtr($7)); }
    | SWITCH '(' expression ')' statement { $$ = new SwitchStatement(ExpressionPtr($3), StatementPtr($5)); }
    ;

iteration_statement
    : WHILE '(' expression ')' statement { $$ = new WhileStatement(ExpressionPtr($3), StatementPtr($5)); }
    | DO statement WHILE '(' expression ')' ';' { $$ = new DoWhileStatement(StatementPtr($2), ExpressionPtr($5)); }
    | FOR '(' expression_statement expression_statement ')' statement { $$ = new ForStatement(ExpressionStatementPtr($3), ExpressionStatementPtr($4), nullptr, StatementPtr($6)); }
    | FOR '(' expression_statement expression_statement expression ')' statement { $$ = new ForStatement(ExpressionStatementPtr($3), ExpressionStatementPtr($4), ExpressionPtr($5), StatementPtr($7)); }
    ;

jump_statement
    : GOTO IDENTIFIER ';' { std::cerr << "goto keyword is unsupported" << std::endl; exit(1); }
	| CONTINUE ';' { $$ = new ContinueStatement(); }
	| BREAK ';' { $$ = new BreakStatement(); }
	| RETURN ';' { $$ = new ReturnStatement(nullptr); }
	| RETURN expression ';' { $$ = new ReturnStatement(ExpressionPtr($2)); }
	;

%%

void yyerror(const char* s)
{
    std::cerr << "Error at line " << yylineno << ": " << s << std::endl;
    std::cerr << "Near token: " << yytext << std::endl;
    exit(1);
}

Node* g_root;
std::unordered_map<std::string, TypeSpecifier> typedefs;

NodePtr ParseAST(std::string file_name)
{
  yyin = fopen(file_name.c_str(), "r");
  if(yyin == nullptr){
    std::cerr << "Couldn't open input file: " << file_name << std::endl;
    exit(1);
  }
  g_root = nullptr;
  yyparse();
  fclose(yyin);
  yylex_destroy();
  return NodePtr(g_root);
}

