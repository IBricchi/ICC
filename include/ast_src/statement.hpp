#pragma once

#include "ast.hpp"
#include "expression.hpp"

/*
    Function return statement.
    The returned expression must have the type of the function.
    Void functions have no expression.
    The main function returns 0 by default.
*/
class AST_Return
    : public AST
{
private:
    AST* expr;

public:
    AST_Return(AST* _expr = nullptr);

    void generateFrames(Frame* _frame = nullptr) override;
    AST* deepCopy() override;
    void compile(std::ostream &assemblyOut) override;

    ~AST_Return();
};

class AST_Break
    : public AST
{
public:
    void generateFrames(Frame* _frame = nullptr) override;
    AST* deepCopy() override;
    void compile(std::ostream &assemblyOut) override;
};

class AST_Continue
    : public AST
{
public:
    void generateFrames(Frame* _frame = nullptr) override;
    AST* deepCopy() override;
    void compile(std::ostream &assemblyOut) override;
};

class AST_IfStmt
    : public AST
{
private:
    AST* cond;
    AST* then;
    AST* other;

public:
    AST_IfStmt(AST* _cond, AST* _then, AST* _other = nullptr);

    void generateFrames(Frame* _frame = nullptr) override;
    AST* deepCopy() override;
    void compile(std::ostream &assemblyOut) override;

    ~AST_IfStmt();
};

/*
    While statement
    Typical while statement loops untill condition is false
*/

class AST_WhileStmt
    : public AST
{
private:
    AST* cond;
    AST* body;

public:
    AST_WhileStmt(AST* _cond, AST* _body);

    void generateFrames(Frame* _frame = nullptr) override;
    AST* deepCopy() override;
    void compile(std::ostream& assemblyOut) override;

    ~AST_WhileStmt();
};

/*
    Block ast node is a statment that can be used to create a new scope.
    It's required by functions, and is very sueful for if statement and loops to put togetehr
    many instructions together
*/

class AST_Block
    : public AST
{
private:
    AST* body;

public:
    AST_Block(AST* _body = nullptr);

    void generateFrames(Frame* _frame = nullptr) override;
    AST* deepCopy() override;
    void compile(std::ostream& assemblyOut) override;

    ~AST_Block();
};
