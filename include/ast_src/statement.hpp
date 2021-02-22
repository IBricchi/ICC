#pragma once

#include "ast.hpp"

class AST_Return{
private:
    AST* expr;
public:
    AST_Return(AST* _expr = nullptr):
        expr(_expr)
    {}

    std::string compile() override{
        return "Not Yet Implemented.\n";
    }

    ~AST_Return(){
        delete expr;
    }
}

class AST_IfStmt : AST {
private:
    AST* cond;
    AST* then;
    AST* other;
public:
    AST_IfStmt(AST* _cond, AST* _then, AST* _other = nullptr):
        cond(_cond),
        then(_then),
        other(_other)
    {}

    std::string compile() override{
        return "Not Implemented Yet.\n";
    }

    ~AST_IfStmt(){
        delete cond;
        delete then;
        delete other;
    }
}