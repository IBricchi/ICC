#pragma once

#include "ast.hpp"
#include "primitive.hpp"

/*
    Var assign should change the value of the variable
    It evaluates the expression expr and assigns the value
    to the variabel with a given name
    We don't need to error check existance of variable name since it should always
    Exists as provided by the specs
*/
class AST_VarAssign
    : public AST
{
private:
    std::string name;
    AST* expr;
public:
    AST_VarAssign(std::string* _name, AST* _expr);

    void generateFrames(Frame* _frame = nullptr) override;
    void compile(std::ostream &assemblyOut) override;

    ~AST_VarAssign();
};

class AST_FunctionCall
    : public AST
{
private:
    std::string functionName;
    std::vector<AST*>* args;
    int parity; // number of arguments

public:
    AST_FunctionCall(std::string* _functionName, std::vector<AST*>* _args = nullptr);

    void generateFrames(Frame* _frame = nullptr) override;
    void compile(std::ostream &assemblyOut) override;

    ~AST_FunctionCall();
};

/*
    Binary operator acts on two inputs to produce a single output
    This is the class for any * / % + - etc.
*/

class AST_BinOp
    : public AST
{
public:
    // the type is important to know instruction to print out in complie
    // however we can treat everything else about the node the same since
    // they all do exactly the same thing
    enum struct Type{
        LOGIC_OR,
        LOGIC_AND,
        BIT_OR,
        BIT_XOR,
        BIT_AND,
        EQUAL_EQUAL, BANG_EQUAL,
        LESS, LESS_EQUAL, GREATER, GREATER_EQUAL,
        SHIFT_L, SHIFT_R,
        PLUS, MINUS,
        STAR, SLASH_F, PERCENT,
        EXP // NOT YET IMPLEMENTED
    };
private:
    Type type;
    AST* left;
    AST* right;
public:
    AST_BinOp(Type _type, AST* _left, AST* _right);

    void generateFrames(Frame* _frame = nullptr) override;
    void compile(std::ostream &assemblyOut) override;

    ~AST_BinOp();
};

class AST_UnOp
    : public AST
{
public:
    // same reasoning as for the bin op
    enum struct Type{
        BANG, NOT, MINUS, PLUS,
        PRE_INCREMENT, PRE_DECREMENT,
        POST_INCREMENT, POST_DECREMENT
    };
private:
    Type type;
    AST* operand;
public:
    AST_UnOp(Type _type, AST* _operand);

    void generateFrames(Frame* _frame = nullptr) override;
    void compile(std::ostream &assemblyOut) override;

    ~AST_UnOp();
};
