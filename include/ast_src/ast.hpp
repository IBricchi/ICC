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
        // constants
        C_STR, C_INT, C_FLOAT
    } type;
    
    std::string value;

    std::vector<AST*> branches;
    
    template<typename val_type>
    AST(TYPE _type, val_type _value, const std::vector<AST*> &_branches)
        : type(_type)
        , value(_value)
        , branches(_branches)
    {}
    
    template<typename val_type>
    AST(TYPE _type, std::string _value)
        : type(_type)
        , value(_value)
    {}
    
    template<class ...TArgs>
    AST(TYPE _type, TArgs ...args)
        : type(_type)
        , branches{args...}
    {}

    ~AST(){
        delete value;

        // delete all nodes
        for(AST* node : branches)
        {
            delete node;
        }
    }
};