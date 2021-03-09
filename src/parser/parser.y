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

// debugging
%define parse.error verbose
%define parse.trace

// Represents the value associated with any kind of
// AST node.
%union{
  AST* NODE ;
  int INT;
  std::string *STR;
  std::vector<std::pair<std::string,std::string>> *FDP; // function declaration parameters
  std::vector<AST*> *FCP; // function call parameters;
}

%token T_INT

%token <STR> T_IDENTIFIER

%token <INT> T_CONST_INT

%token T_RETURN T_IF T_ELSE T_WHILE T_FOR T_BREAK T_CONTINUE

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
%token T_PLUS T_MINUS T_PLUSPLUS T_MINUSMINUS
%token T_STAR T_SLASH_F T_PERCENT
%token T_BANG T_NOT

%type <NODE> PROGRAM SEQUENCE DECLARATION FUN_DECLARATION VAR_DECLARATION // Structures
%type <NODE> STATEMENT EXPRESSION_STMT RETURN_STMT BREAK_STMT CONTINUE_STMT IF_STMT WHILE_STMT FOR_STMT BLOCK // Statements
%type <NODE> EXPRESSION ASSIGNMENT LOGIC_OR LOGIC_AND BIT_OR BIT_XOR BIT_AND // Expressions
%type <NODE> EQUALITY COMPARISON BIT_SHIFT TERM FACTOR UNARY CALL PRIMARY // Expressions

%type <FDP> FUN_DEC_PARAMS // helper for fun declaration
%type <FCP> FUN_CALL_PARAMS // helper for fun call

%nonassoc NO_ELSE
%nonassoc T_ELSE

%nonassoc VAR_DEC
%nonassoc VAR_ASS

%start PROGRAM

%%

// grammar

PROGRAM : SEQUENCE { g_root = $1; }
        ;

SEQUENCE : DECLARATION          {$$ = $1; }
         | DECLARATION SEQUENCE { $$ = new AST_Sequence($1, $2); }
         ;

DECLARATION : FUN_DECLARATION { $$ = $1; }
            | VAR_DECLARATION { $$ = $1; }
            | STATEMENT       { $$ = $1; }
            ;

FUN_DECLARATION : T_INT T_IDENTIFIER T_BRACK_L T_BRACK_R T_SEMI_COLON                                   { $$ = new AST_FunDeclaration("int", $2); }
                | T_INT T_IDENTIFIER T_BRACK_L T_INT T_IDENTIFIER T_BRACK_R T_SEMI_COLON                { $$ = new AST_FunDeclaration("int", $2, nullptr, new std::vector<std::pair<std::string,std::string>>({{"int", *$5}})); }
                | T_INT T_IDENTIFIER T_BRACK_L T_INT T_IDENTIFIER FUN_DEC_PARAMS T_BRACK_R T_SEMI_COLON {
                                $6->push_back({"int", *$5});
                                $$ = new AST_FunDeclaration("int", $2, nullptr, $6);
                        }                            
                | T_INT T_IDENTIFIER T_BRACK_L T_BRACK_R BLOCK                                          { $$ = new AST_FunDeclaration("int", $2, $5); }
                | T_INT T_IDENTIFIER T_BRACK_L T_INT T_IDENTIFIER T_BRACK_R BLOCK                       { $$ = new AST_FunDeclaration("int", $2, $7, new std::vector<std::pair<std::string,std::string>>({{"int", *$5}})); }
                | T_INT T_IDENTIFIER T_BRACK_L T_INT T_IDENTIFIER FUN_DEC_PARAMS T_BRACK_R BLOCK        {
                                $6->push_back({"int", *$5});
                                $$ = new AST_FunDeclaration("int", $2, $8, $6);
                        }
                ;

FUN_DEC_PARAMS : T_COMMA T_INT T_IDENTIFIER                     { $$ = new std::vector<std::pair<std::string,std::string>>({{"int", *$3}}); }
               | T_COMMA T_INT T_IDENTIFIER FUN_DEC_PARAMS      {
                                $4->push_back({"int", *$3});
                                $$ = $4;
                        }
               ;

VAR_DECLARATION : T_INT T_IDENTIFIER T_SEMI_COLON                                { $$ = new AST_VarDeclaration("int", $2); }
                | T_INT T_IDENTIFIER T_EQUAL LOGIC_OR T_SEMI_COLON %prec VAR_DEC { $$ = new AST_VarDeclaration("int", $2, $4); }
                ;

STATEMENT : EXPRESSION_STMT { $$ = $1; }
          | RETURN_STMT     { $$ = $1; }
          | BREAK_STMT      { $$ = $1; }
          | CONTINUE_STMT   { $$ = $1; }
          | IF_STMT         { $$ = $1; }
          | WHILE_STMT      { $$ = $1; }
          | FOR_STMT        { $$ = $1; }
          | BLOCK           { $$ = $1; }
          ;

EXPRESSION_STMT : EXPRESSION T_SEMI_COLON { $$ = $1; }
                ;

RETURN_STMT : T_RETURN T_SEMI_COLON            { $$ = new AST_Return(); }
            | T_RETURN EXPRESSION T_SEMI_COLON { $$ = new AST_Return($2); }
            ;

BREAK_STMT : T_BREAK T_SEMI_COLON { $$ = new AST_Break(); }
           ;

CONTINUE_STMT : T_CONTINUE T_SEMI_COLON { $$ = new AST_Continue(); }
              ;

IF_STMT : T_IF T_BRACK_L EXPRESSION T_BRACK_R STATEMENT    %prec NO_ELSE { $$ = new AST_IfStmt($3, $5); }
        | T_IF T_BRACK_L EXPRESSION T_BRACK_R STATEMENT T_ELSE STATEMENT { $$ = new AST_IfStmt($3, $5, $7); }
        ;

WHILE_STMT : T_WHILE T_BRACK_L EXPRESSION T_BRACK_R STATEMENT { $$ = new AST_WhileStmt($3, $5); }
           ;

FOR_STMT : T_FOR T_BRACK_L EXPRESSION_STMT EXPRESSION_STMT EXPRESSION T_BRACK_R DECLARATION 
                { 
                        // Source translation of for loop into sequence
                        AST* whileBodyContents = new AST_Sequence($5, $7);
                        AST* whileBody = new AST_Block(whileBodyContents);
                        AST* whileStmt = new AST_WhileStmt($4, whileBody);

                        $$ = new AST_Sequence($3, whileStmt);
                }
         ;

BLOCK : T_BRACE_L T_BRACE_R          { $$ = new AST_Block(); }
      | T_BRACE_L SEQUENCE T_BRACE_R { $$ = new AST_Block($2); }
      ;

EXPRESSION : ASSIGNMENT { $$ = $1; }
           ;

ASSIGNMENT : T_IDENTIFIER T_EQUAL LOGIC_OR %prec VAR_ASS        { $$ = new AST_VarAssign($1, $3); }
            // do source translation for all shorthand assigns
           | T_IDENTIFIER T_OR_B T_EQUAL LOGIC_OR %prec VAR_ASS {
                        AST* left_var = new AST_Variable($1);
                        AST* operation = new AST_BinOp(AST_BinOp::Type::BIT_OR, left_var, $4);
                        $$ = new AST_VarAssign($1, operation);
                }
           | T_IDENTIFIER T_XOR_B T_EQUAL LOGIC_OR %prec VAR_ASS {
                        AST* left_var = new AST_Variable($1);
                        AST* operation = new AST_BinOp(AST_BinOp::Type::BIT_XOR, left_var, $4);
                        $$ = new AST_VarAssign($1, operation);
                }
           | T_IDENTIFIER T_AND_B T_EQUAL LOGIC_OR %prec VAR_ASS {
                        AST* left_var = new AST_Variable($1);
                        AST* operation = new AST_BinOp(AST_BinOp::Type::BIT_AND, left_var, $4);
                        $$ = new AST_VarAssign($1, operation);
                }
           | T_IDENTIFIER T_SHIFT_L T_EQUAL LOGIC_OR %prec VAR_ASS {
                        AST* left_var = new AST_Variable($1);
                        AST* operation = new AST_BinOp(AST_BinOp::Type::SHIFT_L, left_var, $4);
                        $$ = new AST_VarAssign($1, operation);
                }
           | T_IDENTIFIER T_SHIFT_R T_EQUAL LOGIC_OR %prec VAR_ASS {
                        AST* left_var = new AST_Variable($1);
                        AST* operation = new AST_BinOp(AST_BinOp::Type::SHIFT_R, left_var, $4);
                        $$ = new AST_VarAssign($1, operation);
                }
           | T_IDENTIFIER T_STAR T_EQUAL LOGIC_OR %prec VAR_ASS {
                        AST* left_var = new AST_Variable($1);
                        AST* operation = new AST_BinOp(AST_BinOp::Type::STAR, left_var, $4);
                        $$ = new AST_VarAssign($1, operation);
                }
           | T_IDENTIFIER T_SLASH_F T_EQUAL LOGIC_OR %prec VAR_ASS {
                        AST* left_var = new AST_Variable($1);
                        AST* operation = new AST_BinOp(AST_BinOp::Type::SLASH_F, left_var, $4);
                        $$ = new AST_VarAssign($1, operation);
                }
           | T_IDENTIFIER T_PERCENT T_EQUAL LOGIC_OR %prec VAR_ASS {
                        AST* left_var = new AST_Variable($1);
                        AST* operation = new AST_BinOp(AST_BinOp::Type::PERCENT, left_var, $4);
                        $$ = new AST_VarAssign($1, operation);
                }
           | T_IDENTIFIER T_PLUS T_EQUAL LOGIC_OR %prec VAR_ASS {
                        AST* left_var = new AST_Variable($1);
                        AST* operation = new AST_BinOp(AST_BinOp::Type::PLUS, left_var, $4);
                        $$ = new AST_VarAssign($1, operation);
                }
           | T_IDENTIFIER T_MINUS T_EQUAL LOGIC_OR %prec VAR_ASS {
                        AST* left_var = new AST_Variable($1);
                        AST* operation = new AST_BinOp(AST_BinOp::Type::MINUS, left_var, $4);
                        $$ = new AST_VarAssign($1, operation);
                }
           | LOGIC_OR                                           { $$ = $1; }
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
       | UNARY T_MINUSMINUS     { $$ = new AST_UnOp(AST_UnOp::Type::POST_DECREMENT, $1); }
       | UNARY T_PLUSPLUS       { $$ = new AST_UnOp(AST_UnOp::Type::POST_INCREMENT, $1); }
       | UNARY                  { $$ = $1; }
       ;

UNARY : T_BANG UNARY          { $$ = new AST_UnOp(AST_UnOp::Type::BANG, $2); }
      | T_NOT UNARY           { $$ = new AST_UnOp(AST_UnOp::Type::NOT, $2); }
      | T_MINUS UNARY         { $$ = new AST_UnOp(AST_UnOp::Type::MINUS, $2); }
      | T_MINUSMINUS UNARY    { $$ = new AST_UnOp(AST_UnOp::Type::PRE_DECREMENT, $2); }
      | T_PLUS UNARY          { $$ = new AST_UnOp(AST_UnOp::Type::PLUS, $2); }
      | T_PLUSPLUS UNARY      { $$ = new AST_UnOp(AST_UnOp::Type::PRE_INCREMENT, $2); }
      | CALL                  { $$ = $1; }
      ;

CALL : T_IDENTIFIER T_BRACK_L T_BRACK_R                            { $$ = new AST_FunctionCall($1); }
     | T_IDENTIFIER T_BRACK_L EXPRESSION T_BRACK_R                 { $$ = new AST_FunctionCall($1, new std::vector<AST*>({{$3}})); }
     | T_IDENTIFIER T_BRACK_L EXPRESSION FUN_CALL_PARAMS T_BRACK_R {
             $4->push_back($3);
             $$ = new AST_FunctionCall($1, $4);
        }
     | PRIMARY                                                     { $$ = $1; }
     ;

FUN_CALL_PARAMS : T_COMMA EXPRESSION                 { $$ = new std::vector<AST*>({{$2}}); }
                | T_COMMA EXPRESSION FUN_CALL_PARAMS {
                                $3->push_back($2);
                                $$ = $3;
                        }
                ;

PRIMARY : T_CONST_INT                    { $$ = new AST_ConstInt($1); }
        | T_IDENTIFIER                   { $$ = new AST_Variable($1); }
        | T_BRACK_L EXPRESSION T_BRACK_R { $$ = $2; }
        ;

%%

AST *g_root; // Definition of variable (to match declaration earlier)

AST *parseAST()
{
    // for debugging
    /* yydebug = 1; */
    g_root=0;
    yyparse();
    return g_root;
}