%code requires{
  #include <ast>
  #include <cassert>
  #include <vector>
  #include <utility>

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
  float FLOAT;
  double DOUBLE;
  char CHAR;
  std::string *STR;
  std::vector<std::pair<AST*,std::string>> *FDP; // function declaration parameters
  std::vector<AST*> *FCP; // function call parameters
  std::vector<int> *SCP; // square chain parameters
  std::vector<std::pair<std::string, int>> *EL; // enum list (identifier value mapping)
  std::pair<std::string, int> *EN; // enum
}

%token <STR> T_TYPE

%token <STR> T_IDENTIFIER

%token <INT> T_CONST_INT
%token <FLOAT> T_CONST_FLOAT
%token <DOUBLE> T_CONST_DOUBLE
%token <CHAR> T_CONST_CHAR

%token T_RETURN T_IF T_ELSE T_WHILE T_FOR T_SWITCH T_BREAK 
%token T_CONTINUE T_CASE T_DEFAULT T_ENUM T_SIZEOF

%token T_COMMA T_SEMI_COLON T_COLON
%token T_BRACK_L T_BRACK_R
%token T_BRACE_L T_BRACE_R
%token T_SQUARE_L T_SQUARE_R

%token T_AND_EQUAL T_XOR_EQUAL T_OR_EQUAL T_SHIFT_L_EQUAL T_SHIFT_R_EQUAL T_STAR_EQUAL
%token T_SLASH_F_EQUAL T_PERCENT_EQUAL T_PLUS_EQUAL T_MINUS_EQUAL 
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
%type <NODE> TYPE // helper for anything with type
%type <NODE> STATEMENT EXPRESSION_STMT RETURN_STMT BREAK_STMT CONTINUE_STMT // Statements
%type <NODE> IF_STMT WHILE_STMT FOR_STMT SWITCH_STMT CASE_STMT BLOCK // Statements
%type <NODE> ENUM_DECLARATION // Statements
%type <NODE> EXPRESSION ASSIGNMENT LOGIC_OR LOGIC_AND BIT_OR BIT_XOR BIT_AND // Expressions
%type <NODE> EQUALITY COMPARISON BIT_SHIFT TERM FACTOR UNARY_PRE UNARY_POST CALL SIZEOF PRIMARY // Expressions

%type <EN> ENUM
%type <EL> ENUM_LIST

%type <FDP> FUN_DEC_PARAMS // helper for fun declaration
%type <FCP> FUN_CALL_PARAMS // helper for fun call
%type <SCP> SQUARE_CHAIN // helper for array declarations

%nonassoc NO_ELSE
%nonassoc T_ELSE

%nonassoc VAR_DEC
%nonassoc VAR_ASS

%start PROGRAM

%%

// grammar

PROGRAM : SEQUENCE { g_root = $1; }
        ;

SEQUENCE : DECLARATION          { $$ = $1; }
         | DECLARATION SEQUENCE { $$ = new AST_Sequence($1, $2); }
         ;

DECLARATION : FUN_DECLARATION  { $$ = $1; }
            | VAR_DECLARATION  { $$ = $1; }
            | ENUM_DECLARATION { $$ = $1; }
            | STATEMENT        { $$ = $1; }
            ;

FUN_DECLARATION : TYPE T_IDENTIFIER T_BRACK_L T_BRACK_R T_SEMI_COLON                                  { $$ = new AST_FunDeclaration($1, $2); }
                | TYPE T_IDENTIFIER T_BRACK_L TYPE T_IDENTIFIER T_BRACK_R T_SEMI_COLON                { $$ = new AST_FunDeclaration($1, $2, nullptr, new std::vector<std::pair<AST*,std::string>>({{$4, *$5}})); }
                | TYPE T_IDENTIFIER T_BRACK_L TYPE T_IDENTIFIER FUN_DEC_PARAMS T_BRACK_R T_SEMI_COLON {
                                $6->push_back({$4, *$5});
                                $$ = new AST_FunDeclaration($1, $2, nullptr, $6);
                        }                            
                | TYPE T_IDENTIFIER T_BRACK_L T_BRACK_R BLOCK                                         { $$ = new AST_FunDeclaration($1, $2, $5); }
                | TYPE T_IDENTIFIER T_BRACK_L TYPE T_IDENTIFIER T_BRACK_R BLOCK                       { $$ = new AST_FunDeclaration($1, $2, $7, new std::vector<std::pair<AST*,std::string>>({{$4, *$5}})); }
                | TYPE T_IDENTIFIER T_BRACK_L TYPE T_IDENTIFIER FUN_DEC_PARAMS T_BRACK_R BLOCK        {
                                $6->push_back({$4, *$5});
                                $$ = new AST_FunDeclaration($1, $2, $8, $6);
                        }
                ;

FUN_DEC_PARAMS : T_COMMA TYPE T_IDENTIFIER                     { $$ = new std::vector<std::pair<AST*,std::string>>({{$2, *$3}}); }
               | T_COMMA TYPE T_IDENTIFIER FUN_DEC_PARAMS      {
                                $4->push_back({$2, *$3});
                                $$ = $4;
                        }
               ;

VAR_DECLARATION : TYPE T_IDENTIFIER T_SEMI_COLON                                   { $$ = new AST_VarDeclaration($1, $2); }
                | TYPE T_IDENTIFIER T_EQUAL LOGIC_OR T_SEMI_COLON %prec VAR_DEC    { $$ = new AST_VarDeclaration($1, $2, $4); }
                | TYPE T_IDENTIFIER SQUARE_CHAIN T_SEMI_COLON {
                                AST* type = new AST_ArrayType($1, $3->at($3->size()-1));
                                for(int i = $3->size() - 2; i >= 0; i--){
                                        type = new AST_ArrayType(type, $3->at(i));
                                }
                                $$ = new AST_ArrayDeclaration(type, $2);
                        }
                ;

SQUARE_CHAIN : T_SQUARE_L T_CONST_INT T_SQUARE_R              { $$ = new std::vector<int>({$2}); }
             | SQUARE_CHAIN T_SQUARE_L T_CONST_INT T_SQUARE_R {
                     $1->push_back($3);
                     $$ = $1;
                }
             ;

SIZEOF : T_SIZEOF T_BRACK_L TYPE T_BRACK_R    { $$ = new AST_Sizeof($3); }
       | T_SIZEOF T_BRACK_L PRIMARY T_BRACK_R { $$ = new AST_Sizeof($3); } // PRIMARY must be a variable
       ;

TYPE : T_TYPE { $$ = new AST_Type($1); }

ENUM_DECLARATION : T_ENUM T_IDENTIFIER T_BRACE_L ENUM_LIST T_BRACE_R T_SEMI_COLON {
                                int count = 0;
                                std::vector<AST*> declarations{};
                                for (auto el : *$4) {
                                        if (el.second != 0) {
                                                count = el.second;
                                        }
                                        std::string* intTypeName = new std::string("int");
                                        AST* intType = new AST_Type(intTypeName);
                                        AST* val = new AST_ConstInt(count);
                                        AST* dec = new AST_VarDeclaration(intType, &el.first, val);
                                        declarations.push_back(dec);
                                        count++;
                                }

                                AST* seq = declarations.at(0);
                                for (int i=1; i<declarations.size(); i++) {
                                        seq = new AST_Sequence(declarations.at(i), seq);
                                }
                                $$ = seq;
                        }
                 | T_ENUM T_IDENTIFIER T_IDENTIFIER T_SEMI_COLON {
                                std::string* intTypeName = new std::string("int");
                                AST* intType = new AST_Type(intTypeName);
                                AST* zero = new AST_ConstInt(0);
                                $$ = new AST_VarDeclaration(intType, $3, zero); 
                        }
                 | T_ENUM T_BRACE_L ENUM_LIST T_BRACE_R T_SEMI_COLON {
                                int count = 0;
                                std::vector<AST*> declarations{};
                                for (auto el : *$3) {
                                        if (el.second != 0) {
                                                count = el.second;
                                        }
                                        std::string* intTypeName = new std::string("int");
                                        AST* intType = new AST_Type(intTypeName);
                                        AST* val = new AST_ConstInt(count);
                                        AST* dec = new AST_VarDeclaration(intType, &el.first, val);
                                        declarations.push_back(dec);
                                        count++;
                                }

                                AST* seq = declarations.at(0);
                                for (int i=1; i<declarations.size(); i++) {
                                        seq = new AST_Sequence(declarations.at(i), seq);
                                }
                                $$ = seq;
                        }
                 ;
                
ENUM_LIST : ENUM                             { $$ = new std::vector<std::pair<std::string, int>>({*$1}); }
          | ENUM_LIST T_COMMA ENUM           { 
                        $1->push_back(*$3);
                        $$ = $1;
                }
          ;

ENUM : T_IDENTIFIER T_EQUAL T_CONST_INT { $$ = new std::pair<std::string, int>({*$1, $3}); }
     | T_IDENTIFIER                     { $$ = new std::pair<std::string, int>({*$1, 0}); }
     ;

STATEMENT : EXPRESSION_STMT { $$ = $1; }
          | RETURN_STMT     { $$ = $1; }
          | BREAK_STMT      { $$ = $1; }
          | CONTINUE_STMT   { $$ = $1; }
          | IF_STMT         { $$ = $1; }
          | WHILE_STMT      { $$ = $1; }
          | FOR_STMT        { $$ = $1; }
          | SWITCH_STMT     { $$ = $1; }
          | CASE_STMT       { $$ = $1; }
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

FOR_STMT : T_FOR T_BRACK_L EXPRESSION_STMT EXPRESSION_STMT EXPRESSION T_BRACK_R STATEMENT 
                { 
                        // Source translation of for loop into sequence
                        AST* whileBodyContents = new AST_Sequence($7, $5);
                        AST* whileBody = new AST_Block(whileBodyContents);
                        AST* whileStmt = new AST_WhileStmt($4, whileBody);

                        $$ = new AST_Sequence($3, whileStmt);
                }
         ;

SWITCH_STMT : T_SWITCH T_BRACK_L EXPRESSION T_BRACK_R STATEMENT { $$ = new AST_SwitchStmt($3, $5); }
            ;

CASE_STMT : T_CASE T_CONST_INT T_COLON STATEMENT { $$ = new AST_CaseStmt($4, $2); }
          | T_DEFAULT T_COLON STATEMENT { $$ = new AST_CaseStmt($3); }
          ;

BLOCK : T_BRACE_L T_BRACE_R          { $$ = new AST_Block(); }
      | T_BRACE_L SEQUENCE T_BRACE_R { $$ = new AST_Block($2); }
      ;

EXPRESSION : ASSIGNMENT { $$ = $1; }
           ;

// do source translation for all shorthand assigns
ASSIGNMENT : UNARY_PRE T_AND_EQUAL LOGIC_OR %prec VAR_ASS {
                        AST* assignee_copy = $1->deepCopy();
                        AST* operation = new AST_BinOp(AST_BinOp::Type::BIT_AND, assignee_copy, $3);
                        $$ = new AST_Assign($1, operation);
                }
           | UNARY_PRE T_XOR_EQUAL LOGIC_OR %prec VAR_ASS {
                        AST* assignee_copy = $1->deepCopy();
                        AST* operation = new AST_BinOp(AST_BinOp::Type::BIT_XOR, assignee_copy, $3);
                        $$ = new AST_Assign($1, operation);
                }
           | UNARY_PRE T_OR_EQUAL LOGIC_OR %prec VAR_ASS {
                        AST* assignee_copy = $1->deepCopy();
                        AST* operation = new AST_BinOp(AST_BinOp::Type::BIT_OR, assignee_copy, $3);
                        $$ = new AST_Assign($1, operation);
                }
           | UNARY_PRE T_SHIFT_L_EQUAL LOGIC_OR %prec VAR_ASS {
                        AST* assignee_copy = $1->deepCopy();
                        AST* operation = new AST_BinOp(AST_BinOp::Type::SHIFT_L, assignee_copy, $3);
                        $$ = new AST_Assign($1, operation);
                }
           | UNARY_PRE T_SHIFT_R_EQUAL LOGIC_OR %prec VAR_ASS {
                        AST* assignee_copy = $1->deepCopy();
                        AST* operation = new AST_BinOp(AST_BinOp::Type::SHIFT_R, assignee_copy, $3);
                        $$ = new AST_Assign($1, operation);
                }
           | UNARY_PRE T_STAR_EQUAL LOGIC_OR %prec VAR_ASS {
                        AST* assignee_copy = $1->deepCopy();
                        AST* operation = new AST_BinOp(AST_BinOp::Type::STAR, assignee_copy, $3);
                        $$ = new AST_Assign($1, operation);
                }
           | UNARY_PRE T_SLASH_F_EQUAL LOGIC_OR %prec VAR_ASS {
                        AST* assignee_copy = $1->deepCopy();
                        AST* operation = new AST_BinOp(AST_BinOp::Type::SLASH_F, assignee_copy, $3);
                        $$ = new AST_Assign($1, operation);
                }
           | UNARY_PRE T_PERCENT_EQUAL LOGIC_OR %prec VAR_ASS {
                        AST* assignee_copy = $1->deepCopy();
                        AST* operation = new AST_BinOp(AST_BinOp::Type::PERCENT, assignee_copy, $3);
                        $$ = new AST_Assign($1, operation);
                }
           | UNARY_PRE T_PLUS_EQUAL LOGIC_OR %prec VAR_ASS {
                        AST* assignee_copy = $1->deepCopy();
                        AST* operation = new AST_BinOp(AST_BinOp::Type::PLUS, assignee_copy, $3);
                        $$ = new AST_Assign($1, operation);
                }
           | UNARY_PRE T_MINUS_EQUAL LOGIC_OR %prec VAR_ASS {
                        AST* assignee_copy = $1->deepCopy();
                        AST* operation = new AST_BinOp(AST_BinOp::Type::MINUS, assignee_copy, $3);
                        $$ = new AST_Assign($1, operation);
                }
           // normal assign
           | UNARY_PRE T_EQUAL LOGIC_OR %prec VAR_ASS { $$ = new AST_Assign($1, $3); }
           | LOGIC_OR                                 { $$ = $1; }
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

FACTOR : UNARY_PRE T_STAR FACTOR    { $$ = new AST_BinOp(AST_BinOp::Type::STAR, $1, $3); }
       | UNARY_PRE T_SLASH_F FACTOR { $$ = new AST_BinOp(AST_BinOp::Type::SLASH_F, $1, $3); }
       | UNARY_PRE T_PERCENT FACTOR { $$ = new AST_BinOp(AST_BinOp::Type::PERCENT, $1, $3); }
       | UNARY_PRE                  { $$ = $1; }
       ;

UNARY_PRE : T_BANG UNARY_PRE          { $$ = new AST_UnOp(AST_UnOp::Type::BANG, $2); }
          | T_NOT UNARY_PRE           { $$ = new AST_UnOp(AST_UnOp::Type::NOT, $2); }
          | T_MINUS UNARY_PRE         { $$ = new AST_UnOp(AST_UnOp::Type::MINUS, $2); }
          | T_MINUSMINUS UNARY_PRE    { $$ = new AST_UnOp(AST_UnOp::Type::PRE_DECREMENT, $2); }
          | T_PLUS UNARY_PRE          { $$ = new AST_UnOp(AST_UnOp::Type::PLUS, $2); }
          | T_PLUSPLUS UNARY_PRE      { $$ = new AST_UnOp(AST_UnOp::Type::PRE_INCREMENT, $2); }
          | UNARY_POST                { $$ = $1; }
          ;

UNARY_POST : UNARY_POST T_MINUSMINUS    { $$ = new AST_UnOp(AST_UnOp::Type::POST_DECREMENT, $1); }
           | UNARY_POST T_PLUSPLUS      { $$ = new AST_UnOp(AST_UnOp::Type::POST_INCREMENT, $1); }
           | CALL                       { $$ = $1; }
           ;

CALL : T_IDENTIFIER T_BRACK_L T_BRACK_R                            { $$ = new AST_FunctionCall($1); }
     | T_IDENTIFIER T_BRACK_L EXPRESSION T_BRACK_R                 { $$ = new AST_FunctionCall($1, new std::vector<AST*>({{$3}})); }
     | T_IDENTIFIER T_BRACK_L EXPRESSION FUN_CALL_PARAMS T_BRACK_R {
             $4->push_back($3);
             $$ = new AST_FunctionCall($1, $4);
        }
     | CALL T_SQUARE_L EXPRESSION T_SQUARE_R                       { $$ = new AST_BinOp(AST_BinOp::Type::ARRAY, $1, $3); }
     | PRIMARY                                                     { $$ = $1; }
     ;

FUN_CALL_PARAMS : T_COMMA EXPRESSION                 { $$ = new std::vector<AST*>({{$2}}); }
                | T_COMMA EXPRESSION FUN_CALL_PARAMS {
                                $3->push_back($2);
                                $$ = $3;
                        }
                ;

PRIMARY : T_CONST_INT                    { $$ = new AST_ConstInt($1); }
        | T_CONST_FLOAT                  { $$ = new AST_ConstFloat($1); }
        | T_CONST_DOUBLE                 { $$ = new AST_ConstDouble($1); }
        | T_CONST_CHAR                   { $$ = new AST_ConstChar($1); }
        | T_IDENTIFIER                   { $$ = new AST_Variable($1); }
        | SIZEOF                         { $$ = $1; }
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