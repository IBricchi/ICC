#pragma once

#include "ast.hpp"

class AST_Constant
    : public AST
{
private:
    AST* value;

public:
    AST_Constant(AST* _value);

    void compile(std::ostream &assemblyOut, Frame &frame) override;

    ~AST_Constant();
}

/*
    Not sure if we need this. 
    Maybe best to replace with an "AST_Assignment" class that represents this
    with attributes "identifier/name" and "value", both of type string.
    This would make it easier to differentiate between declarations and assignments.
*/
class AST_Variable
    : public AST
{
private:
    AST* name;

public:
    AST_Variable(AST* _name);

    void compile(std::ostream &assemblyOut, Frame &frame) override;

    ~AST_Variable();
}
