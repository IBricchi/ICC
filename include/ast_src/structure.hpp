#pragma once

#include <string>
#include "ast.hpp"
#include "expression.hpp"

class AST_Sequence
    : public AST
{
private:
    AST* first;
    AST* second;

public:
    AST_Sequence(AST* _first, AST* _second);

    void generateFrames(Frame* _frame = nullptr) override;
    AST* deepCopy() override;
    void compile(std::ostream &assemblyOut) override;

    ~AST_Sequence();
};

class AST_FunDeclaration
    : public AST
{
private:
    AST* type;
    std::string name;
    AST* body;
    int parity;
    // first in params is type, second is variable name
    std::vector<std::pair<AST*, std::string>>* params;

public:
    /*
        Function body is optional and can be provided in a function definition later on.
    */
    AST_FunDeclaration(AST* type, std::string* _name, AST* _body = nullptr, std::vector<std::pair<AST*,std::string>>* _params = nullptr);

    void generateFrames(Frame* _frame = nullptr) override;
    AST* deepCopy() override;
    void compile(std::ostream &assemblyOut) override;

    ~AST_FunDeclaration();
};

class AST_VarDeclaration
    : public AST
{
private:
    AST* type;
    std::string name;
    AST* expr;

public:
    /*
        Corresponding object is AST_Variable in primitive.hpp.
    */
    AST_VarDeclaration(AST* _type, std::string* _name, AST* _expr = nullptr);

    void generateFrames(Frame* _frame = nullptr) override;
    AST* deepCopy() override;
    void compile(std::ostream &assemblyOut) override;
    AST* getType() override;

    ~AST_VarDeclaration();
};

class AST_ArrayDeclaration
    : public AST
{
private:
    AST* type;
    std::string name;
public:
    AST_ArrayDeclaration(AST* _type, std::string* _name);

    void generateFrames(Frame* _frame = nullptr) override;
    AST* deepCopy() override;
    void compile(std::ostream &assemblyOut) override;

    ~AST_ArrayDeclaration();
};