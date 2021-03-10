%option noyywrap

%{

// Avoid error "error: `fileno' was not declared in this scope"
extern "C" int fileno(FILE *stream);

#include "parser.tab.hpp"
#include <string>
#include <unordered_set>

std::unordered_set<std::string> lexer_types = {
  "int",
  "char",
  "float",
  "double"
};

%}

%option yylineno

%%

"return"   { return T_RETURN; }
"if"       { return T_IF; }
"else"     { return T_ELSE; }
"while"    { return T_WHILE; }
"for"      { return T_FOR; }
"switch"   { return T_SWITCH; }
"break"    { return T_BREAK; }
"continue" { return T_CONTINUE; }
"case"     { return T_CASE; }
"default"  { return T_DEFAULT; }

","       { return T_COMMA; }
";"       { return T_SEMI_COLON; }
":"       { return T_COLON; }
"{"       { return T_BRACE_L; }
"}"       { return T_BRACE_R; }
"("       { return T_BRACK_L; }
")"       { return T_BRACK_R; }
"["       { return T_SQUARE_L; }
"]"       { return T_SQUARE_R; }

"&="      { return T_AND_EQUAL; }
"^="      { return T_XOR_EQUAL; }
"|="      { return T_OR_EQUAL; }
"<<="     { return T_SHIFT_L_EQUAL; }
">>="     { return T_SHIFT_R_EQUAL; }
"*="      { return T_STAR_EQUAL; }
"/="      { return T_SLASH_F_EQUAL; }
"%="      { return T_PERCENT_EQUAL; }
"+="      { return T_PLUS_EQUAL; }
"-="      { return T_MINUS_EQUAL; }
"="       { return T_EQUAL; }

"||"      { return T_OR_L; }
"&&"      { return T_AND_L; }
"|"       { return T_OR_B; }
"^"       { return T_XOR_B; }
"&"       { return T_AND_B; }
"=="      { return T_EQUAL_EQUAL; }
"!="      { return T_BANG_EQUAL; }
"<"       { return T_LESS; }
"<="      { return T_LESS_EQUAL; }
">"       { return T_GREATER; }
">="      { return T_GREATER_EQUAL; }
"<<"      { return T_SHIFT_L; }
">>"      { return T_SHIFT_R; }
"++"      { return T_PLUSPLUS; }
"+"       { return T_PLUS; }
"--"      { return T_MINUSMINUS; }
"-"       { return T_MINUS; }
"*"       { return T_STAR; }
"/"       { return T_SLASH_F; }
"%"       { return T_PERCENT; }
"!"       { return T_BANG; }
"~"       { return T_NOT; }

"//".*                     { /* DO NOTHING (COMMENT) */ }
"\/\*(\*(?!\/)|[^*])*\*\/" { /* DO NOTHING (COMMENT) */ }

[a-zA-Z]([a-zA-Z1-9_])* {
  std::string* s = new std::string(yytext);
  yylval.STR = s;
  if(lexer_types.find(*s) != lexer_types.end())
    return T_TYPE;
  else
    return T_IDENTIFIER;
}
[0-9]+                  { yylval.INT = strtol(yytext, 0, 10); return T_CONST_INT; }

[ \t\r\n]+		{;}

.               { fprintf(stderr, "Invalid token '%s'\n", yytext); exit(1); }

%%

void yyerror (char const *s)
{
  fprintf(stderr,"Parse error | Line: %d\n%s\n", yylineno, s);
  exit(1);
}
