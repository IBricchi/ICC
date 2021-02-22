#pragma once

#include <string>
#include "ast.hpp"

/*
    Base class for ast structures.
    Used for dynamic_cast functionality.
*/
class AST_Structure
    : public AST
{
public:
    virtual ~AST_Structure() = 0;
};

class AST_Sequence
    : public AST_Structure
{
private:
    AST* first;
    AST* second;

public:
    AST_Sequence(AST* _first, AST* _second);

    void compile(std::ostream &assemblyOut) override;

    ~AST_Sequence();
};

class AST_FunDeclaration 
    : public AST_Structure 
{
private:
    std::string type;
    std::string name;
    AST* body;

public:
    /*
        See AST_VarDeclaration below: Need to solve similar problem.
    */
    AST_FunDeclaration(std::string _type, std::string _name, AST* _body);

    void compile(std::ostream &assemblyOut) override;

    ~AST_FunDeclaration();
};

class AST_VarDeclaration 
    : public AST_Structure {
private:
    std::string type; // replace with Enum?
    AST* assignment;

public:
    /*
        Need some kind of central 'bindings' data structure that can be used to 
        get the corresponding value from a variable name.

        Maybe use unordered_map<string, int> where string is the variable name
        and int is the memory address. Then can use "lw $someReg memAddress($fp)"
        to allocate variables at right position relative to frame pointer.
        Double check exact location in lecture 10.
    */
    AST_VarDeclaration(std::string _type, AST* _assignment);

    void compile(std::ostream &assemblyOut) override;

    ~AST_VarDeclaration();
}
