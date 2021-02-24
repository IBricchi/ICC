#pragma once

#include <string>
#include "ast.hpp"

class AST_Sequence
    : public AST
{
private:
    AST* first;
    AST* second;

public:
    AST_Sequence(AST* _first, AST* _second);

    void compile(std::ostream &assemblyOut, Frame &frame) override;

    ~AST_Sequence();
};

class AST_FunDeclaration
    : public AST
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

    void compile(std::ostream &assemblyOut, Frame &frame) override;

    ~AST_FunDeclaration();
};

class AST_VarDeclaration
    : public AST
{
private:
    std::string type; // Necessary? We don't expect errors so don't really care about types.
    AST* assignment; // Should have attributes "identifier/name" and "value", both of type string
    // Add "identifier" attribute of type string? That should match the "identifer" attribute of AST_Assignment.

public:
    /*
        Corresponding object is AST_Variable in primitive.hpp.
    */
    AST_VarDeclaration(std::string _type, AST* _assignment);

    void compile(std::ostream &assemblyOut, Frame &frame) override;

    ~AST_VarDeclaration();
}
