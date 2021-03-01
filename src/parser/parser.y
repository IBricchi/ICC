%code requires{
  #include <ast>
  #include <cassert>

  extern AST *g_root; // A way of getting the AST out

  //! This is to fix problems when generating C++
  // We are declaring the functions provided by Flex, so
  // that Bison generated code can call them.
  int yylex(void);
  void yyerror(const char *);
}

// Represents the value associated with any kind of
// AST node.
%union{
  AST* NODE ;
  int INT;
  std::string *STR;
}

%token T_INT

%token <STR> T_IDENTIFIER

%token <INT> T_CONST_INT

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

%nonassoc NO_ELSE
%nonassoc T_ELSE

%start PROGRAM

%%

// grammar

PROGRAM : SEQUENCE { g_root = $1; }
        ;

SEQUENCE : DECLARATION SEQUENCE { $$ = new AST_Sequence($1, $2); }
         | DECLARATION          {$$ = $1; }
         ;

DECLARATION : FUN_DECLARATION { $$ = $1; }
            | VAR_DECLARATION { $$ = $1; }
            | STATEMENT       { $$ = $1; }
            ;

FUN_DECLARATION : T_INT T_IDENTIFIER T_BRACK_L T_BRACK_R BLOCK { $$ = new AST_FunDeclaration("int", $2, $5); }
                | T_INT T_IDENTIFIER T_BRACK_L T_BRACK_R { $$ = new AST_FunDeclaration("int", $2); }
                ;

VAR_DECLARATION : T_INT T_IDENTIFIER T_SEMI_COLON                  { $$ = new AST_VarDeclaration("int", $2); }
                | T_INT T_IDENTIFIER T_EQUAL LOGIC_OR T_SEMI_COLON { $$ = new AST_VarDeclaration("int", $2, $4); }
                ;

STATEMENT : EXPRESSION_STMT { $$ = $1; }
          | RETURN_STMT     { $$ = $1; }
          | IF_STMT         { $$ = $1; }
          | WHILE_STMT      { $$ = $1; }
          | BLOCK           { $$ = $1; }
          ;

EXPRESSION_STMT : EXPRESSION T_SEMI_COLON { $$ = $1; }
                ;

RETURN_STMT : T_RETURN T_SEMI_COLON            { $$ = new AST_Return(); }
            | T_RETURN EXPRESSION T_SEMI_COLON { $$ = new AST_Return($2); }
            ;

IF_STMT : T_IF T_BRACK_L EXPRESSION T_BRACK_R STATEMENT    %prec NO_ELSE { $$ = new AST_IfStmt($3, $5); }
        | T_IF T_BRACK_L EXPRESSION T_BRACK_R STATEMENT T_ELSE STATEMENT { $$ = new AST_IfStmt($3, $5, $7); }
        ;

WHILE_STMT : T_WHILE T_BRACK_L EXPRESSION T_BRACK_R STATEMENT { $$ = new AST_WhileStmt($3, $5); }
           ;

BLOCK : T_BRACE_L T_BRACE_R          { $$ = new AST_Block(); }
      | T_BRACE_L SEQUENCE T_BRACE_R { $$ = new AST_Block($2); }
      ;

EXPRESSION : ASSIGNMENT { $$ = $1; }
           ;

ASSIGNMENT : T_IDENTIFIER T_EQUAL LOGIC_OR { $$ = new AST_VarAssign($1, $3); }    
           | LOGIC_OR                      { $$ = $1; }
           ;

LOGIC_OR : LOGIC_AND T_OR_L LOGIC_OR { $$ = new AST_BinOp(AST_BinOp::Type::LOGIC_OR, $1, $3); }
         | LOGIC_AND                 { $$ = $1; }
         ;

LOGIC_AND : BIT_OR T_AND_L LOGIC_AND { $$ = new AST_BinOp(AST_BinOp::Type::LOGIC_AND, $1, $3); }
          | BIT_OR                   { $$ = $1; }
          ;

BIT_OR : BIT_XOR T_OR_B BIT_OR { $$ = new AST_BinOp(AST_BinOp::Type::BIT_OR, $1, $3); }
       | BIT_XOR               { $$ = $1; }
       ;

BIT_XOR : BIT_AND T_XOR_B BIT_XOR { $$ = new AST_BinOp(AST_BinOp::Type::BIT_XOR, $1, $3); }
        | BIT_AND                 { $$ = $1; }
        ;

BIT_AND : EQUALITY T_AND_B BIT_AND { $$ = new AST_BinOp(AST_BinOp::Type::BIT_AND, $1, $3); }
        | EQUALITY                 { $$ = $1; }
        ;

EQUALITY : COMPARISON T_EQUAL_EQUAL EQUALITY { $$ = new AST_BinOp(AST_BinOp::Type::EQUAL_EQUAL, $1, $3); }
         | COMPARISON T_BANG_EQUAL EQUALITY  { $$ = new AST_BinOp(AST_BinOp::Type::BANG_EQUAL, $1, $3); }
         | COMPARISON                        { $$ = $1; }
         ;

COMPARISON : BIT_SHIFT T_LESS COMPARISON          { $$ = new AST_BinOp(AST_BinOp::Type::LESS, $1, $3); }
           | BIT_SHIFT T_LESS_EQUAL COMPARISON    { $$ = new AST_BinOp(AST_BinOp::Type::LESS_EQUAL, $1, $3); }
           | BIT_SHIFT T_GREATER COMPARISON       { $$ = new AST_BinOp(AST_BinOp::Type::GREATER, $1, $3); }
           | BIT_SHIFT T_GREATER_EQUAL COMPARISON { $$ = new AST_BinOp(AST_BinOp::Type::GREATER_EQUAL, $1, $3); }
           | BIT_SHIFT                            { $$ = $1; }
           ;

BIT_SHIFT : TERM T_SHIFT_L BIT_SHIFT { $$ = new AST_BinOp(AST_BinOp::Type::SHIFT_L, $1, $3); }
          | TERM T_SHIFT_R BIT_SHIFT { $$ = new AST_BinOp(AST_BinOp::Type::SHIFT_R, $1, $3); }
          | TERM                     { $$ = $1; }
          ;

TERM : FACTOR T_PLUS TERM  { $$ = new AST_BinOp(AST_BinOp::Type::PLUS, $1, $3); }
     | FACTOR T_MINUS TERM { $$ = new AST_BinOp(AST_BinOp::Type::MINUS, $1, $3); }
     | FACTOR              { $$ = $1; }
     ;

FACTOR : UNARY T_STAR FACTOR    { $$ = new AST_BinOp(AST_BinOp::Type::STAR, $1, $3); }
       | UNARY T_SLASH_F FACTOR { $$ = new AST_BinOp(AST_BinOp::Type::SLASH_F, $1, $3); }
       | UNARY T_PERCENT FACTOR { $$ = new AST_BinOp(AST_BinOp::Type::PERCENT, $1, $3); }
       | UNARY                  { $$ = $1; }
       ;

UNARY : T_BANG UNARY  { $$ = new AST_UnOp(AST_UnOp::Type::BANG, $2); }
      | T_NOT UNARY   { $$ = new AST_UnOp(AST_UnOp::Type::NOT, $2); }
      | T_MINUS UNARY { $$ = new AST_UnOp(AST_UnOp::Type::MINUS, $2); }
      | T_PLUS UNARY  { $$ = new AST_UnOp(AST_UnOp::Type::PLUS, $2); }
      | CALL          { $$ = $1; }
      ;

CALL : T_IDENTIFIER T_BRACK_L T_BRACK_R { $$ = new AST_FunctionCall($1); }
     | PRIMARY                          { $$ = $1; }
     ;

PRIMARY : T_CONST_INT                    { $$ = new AST_ConstInt($1); }
        | T_IDENTIFIER                   { $$ = new AST_Variable($1); }
        | T_BRACK_L EXPRESSION T_BRACK_R { $$ = $2; }
        ;

%%

AST *g_root; // Definition of variable (to match declaration earlier)

AST *parseAST()
{
  g_root=0;
  yyparse();
  return g_root;
}
