#pragma once

#include "ast.hpp"

class AST_Sequence : AST{
private:
    AST* first;
    AST* second;
public:
    AST_Sequence(AST* _first, AST* _second):
        first(_first),
        second(_second)
    {}

    std::string compile() override{
        std::string out = first->compile();
        out += second->compile();
    }

    ~AST_Sequence(){
        delete first;
        delete second;
    }
}

class AST_FunDeclaration : AST {
private:
    std::string type;
    std::string name;
    AST* body;
public:
    AST_FunDeclaration(std::string _type, std::string _name, AST* _body):
        type(_type),
        name(_name),
        body(_body)
    {}

    std::string compile() override{
        return "Not Implemented Yet.\n";
    }

    ~AST_FunDeclaration(){
        delete body;
    }
}

class AST_VarDeclaration : AST {
private:
    std::string type;
    AST* assignment;
public:
    AST_VarDeclaration(std::string _type, AST* _assignment):
        type(_type),
        assignment(_assignment)
    {}

    std::string compile() override{
        return "Not Implemented Yet.\n"
    }

    ~AST_VarDeclaration(){
        delete assignment; 
    }
}
