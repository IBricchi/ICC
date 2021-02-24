%option noyywrap

%{

// Avoid error "error: `fileno' was not declared in this scope"
extern "C" int fileno(FILE *stream);

#include "parser.tab.hpp"

%}

%%

"int"     { return T_INT; }

"return"  { return T_RETURN; }
"if"      { return T_IF; }
"else"    { return T_ELSE; }
"while"   { return T_WHILE; }

","       { return T_COMMA; }
";"       { return T_SEMI_COLON; }
"{"       { return T_BRACE_L; }
"}"       { return T_BRACE_R; }

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
"+"       { return T_PLUS; }
"-"       { return T_MINUS; }
"*"       { return T_STAR; }
"/"       { return T_SLASH_F; }
"%"       { return T_PERCENT; }
"!"       { return T_BANG; }
"~"       { return T_NOT; }

[a-zA-Z]([a-zA-Z1-9_])* { yylval.STR = new std::string(yytext); return T_IDENTIFIER; }
[0-9]+                  { yylval.INT = strtoi(yytext, 0); return T_CONST_INT; }

[ \t\r\n]+		{;}

.               { fprintf(stderr, "Invalid token '%s'\n", yytext); exit(1); }

%%

void yyerror (char const *s)
{
  fprintf (stderr, "Parse error : %s\n", s);
  exit(1);
}
