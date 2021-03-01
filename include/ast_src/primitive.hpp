#pragma once

#include "ast.hpp"

/*
    C only supports limited constants so each one will get it's own node
*/
class AST_ConstInt
    : public AST
{
private:
    int value;
public:
    AST_ConstInt(int _value);

    void generateFrames(Frame* _frame = nullptr) override;
    void compile(std::ostream &assemblyOut) override;
    
    // doesn't need a destructor since it holds not pointers
};

/*
   I made the change you suggested by adding an assignment AST node
   This node will be created if an identifier acting as a variable is
   detected in the parser

   I cahnged it to use a string as the name instead of an AST node
*/
class AST_Variable
    : public AST
{
private:
    std::string name;

public:
    AST_Variable(std::string* _name);

    void generateFrames(Frame* _frame = nullptr) override;
    void compile(std::ostream &assemblyOut) override;

    // doesn't need a destructor as there are no pointers
};
