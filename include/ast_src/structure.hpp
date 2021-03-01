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

    void generateFrames(Frame* _frame = nullptr) override;
    void compile(std::ostream &assemblyOut) override;

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
        Function body is optional and can be provided in a function definition later on.
    */
    AST_FunDeclaration(std::string _type, std::string* _name, AST* _body = nullptr);

    void generateFrames(Frame* _frame = nullptr) override;
    void compile(std::ostream &assemblyOut) override;

    ~AST_FunDeclaration();
};

class AST_VarDeclaration
    : public AST
{
private:
    std::string type; // Necessary? We don't expect errors so don't really care about types.
                      // It is necessary since as we add ne wtypes, we're going to have to manage diffrernt sized types
                      // floats will be larger, strings will be even alrger etc.
                      // the compiler will need to know this information
    std::string name; // I replaced the parser from doing T_INT Asgignment to t_int t_identifer t_equal ...
                      // That way we can treat variable definitions and assignments separately
    AST* expr;

public:
    /*
        Corresponding object is AST_Variable in primitive.hpp.
    */
    AST_VarDeclaration(std::string _type, std::string* _name, AST* _expr = nullptr);

    void generateFrames(Frame* _frame = nullptr) override;
    void compile(std::ostream &assemblyOut) override;

    ~AST_VarDeclaration();
};
