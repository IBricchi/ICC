%option noyywrap

%{

// Avoid error "error: `fileno' was not declared in this scope"
extern "C" int fileno(FILE *stream);

#include "maths_parser.tab.hpp"

%}

%%

"int"               { return T_INT; }

[0-9]+              { yylval.number=strtod(yytext, 0); return T_NUMBER; }
[a-z]+              { yylval.string=new std::string(yytext); return T_IDENTIFIER; }

"-"                 { return T_MINUS; }
"+"                 { return T_PLUS; }
"*"                 { return T_STAR; }
"/"                 { return T_SLASH_F; }

"("                 { return T_BRAC_L; }
")"                 { return T_BRAC_R; }
";"                 { return T_SEMI_COLON; }

[ \t\r\n]+		{;}

.               { fprintf(stderr, "Invalid token '%s'\n", yytext); exit(1); }

%%

void yyerror (char const *s)
{
  fprintf (stderr, "Parse error : %s\n", s);
  exit(1);
}