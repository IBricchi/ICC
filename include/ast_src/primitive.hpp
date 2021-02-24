#pragma once

#include "ast.hpp"

class AST_Constant
    : public AST
{
private:
    AST* value; // shouldn't this be a constant like int or something
                // might be useful to just leave it as an int for the first part of the compiler
                // and then as we go along implmenent it differently
                // maybe replace it with AST_C_INT or somethign

public:
    AST_Constant(AST* _value);

    void generateFrames(Frame* _frame = nullptr) override;
    void compile(std::ostream &assemblyOut) override;

    ~AST_Constant();
};

/*
    Not sure if we need this. 
    Maybe best to replace with an "AST_Assignment" class that represents this
    with attributes "identifier/name" and "value", both of type string.
    This would make it easier to differentiate between declarations and assignments.

    I agree, I made a change so that the AST_VarDeclare is different from the AST_VarAssign
    This AST_variable should only be to reprsent a variable in an expression for example
    Also shouldn't name be a string, not sure why it's an AST* at the moment
*/
class AST_Variable
    : public AST
{
private:
    AST* name;

public:
    AST_Variable(AST* _name);

    void generateFrames(Frame* _frame = nullptr) override;
    void compile(std::ostream &assemblyOut) override;

    ~AST_Variable();
};
