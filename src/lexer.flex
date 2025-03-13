%option noyywrap
%option debug
%option yylineno

%{
  // A lot of this lexer is based off the ANSI C grammar:
  // https://www.lysator.liu.se/c/ANSI-C-grammar-l.html#MUL-ASSIGN
  // Avoid error "error: `fileno' was not declared in this scope"
  extern "C" int fileno(FILE *stream);

  #include "parser.tab.hpp"

  // Suppress warning about unused function
  [[maybe_unused]] static void yyunput (int c, char * yy_bp );
%}

D	  [0-9]
L	  [a-zA-Z_]
H   [a-fA-F0-9]
E	  [Ee][+-]?{D}+
FSF (f|F)
FSL (l|L)
IS  (u|U|l|L)*

%%
"/*"([^*]|\*+[^*/])*\*+"/"     { }

"auto"			{return(AUTO);}
"break"			{return(BREAK);}
"case"			{return(CASE);}
"char"			{return(CHAR);}
"const"			{return(CONST);}
"continue"  {return(CONTINUE);}
"default"		{return(DEFAULT);}
"do"			  {return(DO);}
"double"		{return(DOUBLE);}
"else"			{return(ELSE);}
"enum"			{return(ENUM);}
"extern"		{return(EXTERN);}
"float"			{return(FLOAT);}
"for"			  {return(FOR);}
"goto"			{return(GOTO);}
"if"			  {return(IF);}
"int"			  {return(INT);}
"long"			{return(LONG);}
"register"	{return(REGISTER);}
"return"		{return(RETURN);}
"short"			{return(SHORT);}
"signed"		{return(SIGNED);}
"sizeof"		{return(SIZEOF);}
"static"		{return(STATIC);}
"struct"		{return(STRUCT);}
"switch"		{return(SWITCH);}
"typedef"		{return(TYPEDEF);}
"union"			{return(UNION);}
"unsigned"	{return(UNSIGNED);}
"void"			{return(VOID);}
"volatile"	{return(VOLATILE);}
"while"			{return(WHILE);}

{L}({L}|{D})*		{
    yylval.string = new std::string(yytext);
    return typedefs.find(*yylval.string) != typedefs.end() ? TYPE_NAME : IDENTIFIER;
}

0[xX]{H}+{IS}?		{yylval.number_int = (int)strtol(yytext, NULL, 0); return(INT_CONSTANT);}
0{D}+{IS}?		    {yylval.number_int = (int)strtol(yytext, NULL, 0); return(INT_CONSTANT);}
{D}+{IS}?		      {yylval.number_int = (int)strtol(yytext, NULL, 0); return(INT_CONSTANT);}

L?'(\\.|[^\\'])+'	{
  const char *it = yytext;
  assert(*it++ == '\'');
  if (*it == '\\') {
    ++it;
    switch (*it) {
      case 'n': yylval.number_int = '\n'; break;
      case 't': yylval.number_int = '\t'; break;
      case 'v': yylval.number_int = '\v'; break;
      case 'b': yylval.number_int = '\b'; break;
      case 'r': yylval.number_int = '\r'; break;
      case 'f': yylval.number_int = '\f'; break;
      case 'a': yylval.number_int = '\a'; break;
      case '\\': yylval.number_int = '\\'; break;
      case '?': yylval.number_int = '\?'; break;
      case '\'': yylval.number_int = '\''; break;
      case '"': yylval.number_int = '\"'; break;
      case 'x': {
        ++it;
        std::string hex;
        while (*it != '\'') {
          hex += *it++;
        }
        yylval.number_int = (int)strtol(hex.c_str(), NULL, 16);
        break;
      }
      default: {
        std::string oct;
        while (*it != '\'') {
          oct += *it++;
        }
        yylval.number_int = (int)strtol(oct.c_str(), NULL, 8);
        std::cerr << "oct: " << oct << std::endl;
        break;
      }
    }
  } else {
    yylval.number_int = *it;
  }

  return(CHAR_CONSTANT);
}

{D}+{E}?		        {yylval.number_float = strtod(yytext, NULL); return(DOUBLE_CONSTANT);}
{D}*"."{D}+({E})?	    {yylval.number_float = strtod(yytext, NULL); return(DOUBLE_CONSTANT);}
{D}+"."{D}*({E})?   	{yylval.number_float = strtod(yytext, NULL); return(DOUBLE_CONSTANT);}

{D}+{E}{FSF}?		        {yylval.number_float = strtod(yytext, NULL); return(FLOAT_CONSTANT);}
{D}*"."{D}+({E})?{FSF}	{yylval.number_float = strtod(yytext, NULL); return(FLOAT_CONSTANT);}
{D}+"."{D}*({E})?{FSF}	{yylval.number_float = strtod(yytext, NULL); return(FLOAT_CONSTANT);}


L?\"(\\.|[^\\"])*\"	{yylval.string = new std::string(yytext + 1, yyleng - 2); return(STRING_LITERAL);}

"..."      {return(ELLIPSIS);}
">>="			 {return(RIGHT_ASSIGN);}
"<<="      {return(LEFT_ASSIGN);}
"+="			 {return(ADD_ASSIGN);}
"-="       {return(SUB_ASSIGN);}
"*="       {return(MUL_ASSIGN);}
"/="			 {return(DIV_ASSIGN);}
"%="			 {return(MOD_ASSIGN);}
"&="       {return(AND_ASSIGN);}
"^="			 {return(XOR_ASSIGN);}
"|="       {return(OR_ASSIGN);}
">>"       {return(RIGHT_OP);}
"<<"       {return(LEFT_OP);}
"++"			 {return(INC_OP);}
"--"			 {return(DEC_OP);}
"->"			 {return(PTR_OP);}
"&&"			 {return(AND_OP);}
"||"			 {return(OR_OP);}
"<="			 {return(LE_OP);}
">="			 {return(GE_OP);}
"=="			 {return(EQ_OP);}
"!="			 {return(NE_OP);}
";"			   {return(';');}
("{"|"<%") {return('{');}
("}"|"%>") {return('}');}
","			   {return(',');}
":"			   {return(':');}
"="			   {return('=');}
"("		     {return('(');}
")"			   {return(')');}
("["|"<:") {return('[');}
("]"|":>") {return(']');}
"."			   {return('.');}
"&"			   {return('&');}
"!"			   {return('!');}
"~"			   {return('~');}
"-"			   {return('-');}
"+"			   {return('+');}
"*"			   {return('*');}
"/"			   {return('/');}
"%"			   {return('%');}
"<"			   {return('<');}
">"			   {return('>');}
"^"			   {return('^');}
"|"			   {return('|');}
"?"			   {return('?');}

[ \a\b\t\v\f\n\r]		{/* ignore new lines and special sequences */}
.			              {/* ignore bad characters */}

%%
