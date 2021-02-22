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
        throw std::runtime_error("Not Implemented Yet.\n");
    }

    ~AST_Return(){
        delete expr;
    }
}

// Represents both standalone if-statements and combined if-else statements
// Does not yet support if-elseif-else style statements
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
        throw std::runtime_error("Not Implemented Yet.\n");
    }

    ~AST_IfStmt(){
        delete cond;
        delete then;
        delete other;
    }
}
