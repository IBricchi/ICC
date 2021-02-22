%code requires{
  #include <ast>
  #include <cassert>

  extern const AST *g_root; // A way of getting the AST out

  //! This is to fix problems when generating C++
  // We are declaring the functions provided by Flex, so
  // that Bison generated code can call them.
  int yylex(void);
  void yyerror(const char *);
}

// Represents the value associated with any kind of
// AST node.
%union{
  AST *NODE ;
  int INT;
  std::string *STR;
}

%token T_INT

%token T_IDENTIFIER

%token T_CONST_NUM

%token T_RETURN T_IF T_ELSE T_WHILE

%token T_COMMA T_SEMI_COLON
%token T_BRACK_L T_BRACK_R
%token T_BRACE_L T_BRACE_R

%token T_EQUAL
%token T_OR_L
%token T_AND_L
%token T_OR_B
%token T_XOR_B
%token T_AND_B
%token T_EQUAL_EQUAL T_BANG_EQUAL
%token T_LESS T_LESS_EQUAL T_GREATER T_GREATER_EQUAL
%token T_SHIFT_L T_SHIFT_R
%token T_PLUS T_MINUS
%token T_STAR T_SLASH_F T_PERCENT
%token T_BANG T_NOT

%type <NODE> PROGRAM SEQUENCE DECLARATION FUN_DECLARATION VAR_DECLARATION // Structures
%type <NODE> STATEMENT EXPRESSION_STMT RETURN_STMT IF_STMT WHILE_STMT BLOCK // Statements
%type <NODE> EXPRESSION ASSIGNMENT LOGIC_OR LOGIC_AND BIT_OR BIT_XOR BIT_AND // Expressions
%type <NODE> EQUALITY COMPARISON BIT_SHIFT TERM FACTOR UNARY CALL PRIMARY // Expressions

%start ROOT

%%

// grammar

PROGRAM : SEQUENCE { $$ = $1; }
        ;

SEQUENCE : DECLARATION SEQUENCE { $$ = new AST_Sequence($1, $2); }
         | DECLARATION          {$$ = $1; }
         ;

DECLARATION : FUN_DECLARATION { $$ = $1; }
            | VAR_DECLARATION { $$ = $1; }
            | STATEMENT       { $$ = $1; }
            ;

FUN_DECLARATION : T_INT T_IDENTIFIER T_BRACK_L T_BRACK_R BLOCK { $$ = new AST_FunDeclaration("int", $2, $5); }
                ;

VAR_DECLARATION : T_INT ASSIGNMENT { $$ = new AST_VarDeclaration("int", $2); }
                ;

STATEMENT : EXPRESSION_STMT { $$ = $1; }
          | RETURN_STMT     { $$ = $1; }
          | IF_STMT         { $$ = $1; }
          | WHILE_STMT      { $$ = $1; }
          | BLOCK           { $$ = $1; }
          ;

EXPRESSION_STMT : EXPRESSION T_SEMI_COLON { $$ = $1; }
                ;

RETURN_STMT : T_RETURN T_SEMI_COLON { $$ = new AST_Return(); }
            | T_RETURN EXPRESSION T_SEMI_COLON { $$ = new AST_Return($1); }
            ;

IF_STMT : T_IF T_BRACK_L EXPRESSION T_BRACK_R STATEMENT { $$ = new AST_IfStmt($3, $5); }
        | T_IF T_BRACK_L EXPRESSION T_BRACK_R STATEMENT T_ELSE STATEMENT { $$ = new AST_IfStmt($3, $5, $7); }
        ;

WHILE_STMT : T_WHILE T_BRACK_L EXPRESSION T_BRACK_R STATEMENT
           ;

BLOCK : T_BRACE_L T_BRACE_R
      | T_BRACE_L SEQUENCE T_BRACE_R
      ;

EXPRESSION : ASSIGNMENT
           ;

ASSIGNMENT : T_IDENTIFIER T_EQUAL LOGIC_OR
           | LOGIC_OR
           ;

LOGIC_OR : LOGIC_AND T_OR_L LOGIC_OR
         | LOGIC_AND
         ;

LOGIC_AND : BIT_OR T_AND_L LOGIC_AND
          | BIT_OR
          ;

BIT_OR : BIT_XOR T_OR_B BIT_OR
       | BIT_XOR
       ;

BIT_XOR : BIT_AND T_XOR_B BIT_XOR
        | BIT_AND
        ;

BIT_AND : EQUALITY T_AND_B BIT_AND
        | EQUALITY
        ;

EQUALITY : COMPARISON T_EQUAL_EQUAL EQUALITY
         | COMPARISON T_BANG_EQUAL EQUALITY
         | COMPARISON
         ;

COMPARISON : BIT_SHIFT T_LESS COMPARISON
           | BIT_SHIFT T_LESS_EQUAL COMPARISON
           | BIT_SHIFT T_GREATER COMPARISON
           | BIT_SHIFT T_GREATER_EQUAL COMPARISON
           | BIT_SHIFT
           ;

BIT_SHIFT : TERM T_SHIFT_L BIT_SHIFT
          | TERM T_SHIFT_R BIT_SHIFT
          | TERM
          ;

TERM : FACTOR T_PLUS TERM
     | FACTOR T_MINUS TERM
     ;

FACTOR : UNARY T_STAR FACTOR
       | UNARY T_SLASH_F FACTOR
       | UNARY T_PERCENT FACTOR
       | UNARY
       ;

UNARY : T_BANG UNARY
      | T_NOT UNARY
      | T_MINUS UNARY
      | T_PLUS UNARY
      | CALL
      ;

CALL : PRIMARY T_BRACK_L T_BRACK_R
     | PRIMARY
     ;

PRIMARY : T_CONST_NUM
        | T_IDENTIFIER
        | T_BRACK_L EXPRESSION T_BRACK_R
        ;

%%

const AST *g_root; // Definition of variable (to match declaration earlier)

const AST *parseAST()
{
  g_root=0;
  yyparse();
  return g_root;
}
