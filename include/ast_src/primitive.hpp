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

// I'm not sure what the idea of this constant is, having the value be an
// AST node
// I made the above cosntant for only int because I wanted to finish the parser
// file and wasn't really sure what was going on with this one
// the way I see it I think each constant should have it's own node type
// c only supports some numeric types, and c style strings, you can't make a constant
// of a custom type, so we can explicitly handle all types individually
// otherwise I'm not sure how we would manage to compiile the values to assembly
// I've left the class here cause I'm not sure if I just misunderstood what it was
// meant to do
class AST_Constant
    : public AST
{
private:
    AST* value;

public:
    AST_Constant(AST* _value);

    void generateFrames(Frame* _frame = nullptr) override;
    void compile(std::ostream &assemblyOut) override;

    ~AST_Constant();
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
    AST_Variable(std::string& _name);

    void generateFrames(Frame* _frame = nullptr) override;
    void compile(std::ostream &assemblyOut) override;

    // doesn't ened a destrcutor as there are no pointers
};
