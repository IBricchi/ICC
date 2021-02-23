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
