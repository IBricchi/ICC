%option noyywrap

%{

// Avoid error "error: `fileno' was not declared in this scope"
extern "C" int fileno(FILE *stream);

#include "parser.tab.hpp"
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>

// map from type to set of aliases
std::unordered_map<std::string, std::unordered_set<std::string>> lexer_types = {
  {"int",{}},
  {"char",{}},
  {"float",{}},
  {"double",{}},
  {"unsigned",{}},
  {"void",{}}
};
std::unordered_map<std::string, std::unordered_set<std::string>> lexer_pointerTypes = {
  {"int",{}},
  {"char",{}},
  {"float",{}},
  {"double",{}},
  {"unsigned",{}},
  {"void",{}},
};

// map from struct name to set of member (name, type) pairs
std::unordered_map<std::string, std::map<std::string, std::string>> lexer_structs = {};

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
"enum"     { return T_ENUM; }
"struct"   { return T_STRUCT; }
"sizeof"   { return T_SIZEOF; }
"typedef"  { return T_TYPEDEF; }

"unsigned int" {
  std::string s = std::string(yytext);
  s = s.substr(0,s.find(" "));
  std::string* ps = new std::string(s);
  yylval.STR = ps;
  return T_TYPE;
}

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

[a-zA-Z]([a-zA-Z1-9_\.])* {
  std::string* s = new std::string(yytext);
  yylval.STR = s;
  for (auto el : lexer_types) {
    if (el.first == *s) {
      return T_TYPE;
    } else if (el.second.find(*s) != el.second.end()) {
      yylval.STR = new std::string(el.first);
      return T_TYPE;
    }
  }
  for (auto el : lexer_pointerTypes) {
    if (el.second.find(*s) != el.second.end()) {
      yylval.STR = new std::string(el.first);
      return T_POINTERTYPE;
    }
  }
  return T_IDENTIFIER;
}

\'.{1,2}\' {
  // Remove single quotes
  yytext++;
  yytext[strlen(yytext)-1] = 0;
  yylval.CHAR = *yytext;
  return T_CONST_CHAR;
}

[0-9]+                           { yylval.INT = strtol(yytext, 0, 10); return T_CONST_INT; }
([0-9]+([.][0-9]*)?|[.][0-9]+)f  { yylval.FLOAT = std::stof(yytext); return T_CONST_FLOAT; }
[0-9]+([.][0-9]*)?|[.][0-9]+     { yylval.DOUBLE = std::stod(yytext); return T_CONST_DOUBLE; }

[ \t\r\n]+		{;}

.               { fprintf(stderr, "Invalid token '%s'\n", yytext); exit(1); }

%%

void yyerror (char const *s)
{
  fprintf(stderr,"Parse error | Line: %d\n%s\n", yylineno, s);
  exit(1);
}
