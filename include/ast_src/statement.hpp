#pragma once

#include "ast.hpp"

/*
    Base class for ast statements.
    Used for dynamic_cast functionality.
*/
class AST_Statement
    : public AST
{
public:
    virtual ~AST_Statement() = 0;
};

/*
    Function return statement.
    The returned expression must have the type of the function.
    Void functions have no expression.
    The main function returns 0 by default.
*/
class AST_Return 
    : public AST_Statement
{
private:
    AST* expr;

public:
    AST_Return(AST* _expr = nullptr);

    void compile(std::ostream &assemblyOut) override;

    ~AST_Return();
}

/*
    Represents both standalone if-statements and combined if-else statements
    Does not yet support if-elseif-else style statements
*/
class AST_IfStmt 
    : public AST_Statement 
{
private:
    AST* cond;
    AST* then;
    AST* other;

public:
    AST_IfStmt(AST* _cond, AST* _then, AST* _other = nullptr);

    void compile(std::ostream &assemblyOut) override;

    ~AST_IfStmt();
}
