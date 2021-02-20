#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <map>

struct AST;

struct AST
{
    enum struct TYPE{
        // structure
        SEQUENCE, DECLARATION
        // delcaration
        FUN_DECLARATION, VAR_DECLARATION
        // statement
        STATEMENT, EXPRESSION_STMT, RETURN_STMT,
        IF_STMT, WHILE_STMT, BLOCK,
        // expression

        // constants
        C_STR, C_INT, C_FLOAT
    } type;
    
    std::string value;
    std::string value2;

    std::vector<AST*> branches;
        
    AST(TYPE _type, std::string _value)
        : type(_type)
        , value(_value)
    {}
    
    template<class ...TArgs>
    AST(TYPE _type, TArgs ...args)
        : type(_type)
        , branches{args...}
    {}

    template<class ...TArgs>
    AST(TYPE _type, std::string _value, TArgs ...args)
        : type(_type)
        , value(_value)
        , branches{args...}
    {}

    template<class ...TArgs>
    AST(TYPE, _type, std::string _value2, std::string _value, TArgs ...args)
        : type(_type)
        , value(_value)
        , value2(_value2)
        , branches{args...}

    ~AST(){
        delete value;

        // delete all nodes
        for(AST* node : branches)
        {
            delete node;
        }
    }
};