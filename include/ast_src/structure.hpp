#pragma once

#include "ast.hpp"

static AST* CreateSequence(AST* declaration, AST* sequence){
    return new AST(AST::TYPE::SEQUENCE, declaration, sequence);
}

static AST* CreateFunDeclaration(std::string type, std::string name, AST* body){
    return new AST(AST::TYPE::FUN_DECLARATION, type, name, body);
}

static AST* CreateVarDeclaration(std::string var_type, AST* value){
    return new AST(AST::TYPE::VAR_DECLARATION, var_type, value);
}