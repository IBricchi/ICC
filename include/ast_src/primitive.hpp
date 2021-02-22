#pragma once

#include "ast.hpp"

/*
    Base class for ast primitives.
    Used for dynamic_cast functionality.
*/
class AST_Primitive
    : public AST
{
public:
    virtual ~AST_Primitive() = 0;
};

class AST_Constant
    : public AST_Primitive 
{
private:
    AST* value;

public:
    AST_Constant(AST* _value);

    void compile(std::ostream &assemblyOut) override;

    ~AST_Constant();
}

class AST_Variable
    : public AST_Primitive 
{
private:
    AST* name;

public:
    AST_Variable(AST* _name);

    void compile(std::ostream &assemblyOut) override;

    ~AST_Variable();
}
