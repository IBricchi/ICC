#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <stdexcept>

/*
    Base class for all ast nodes
*/
class AST
{
protected:
    AST();

public:
    virtual ~AST() = 0;
    /*
        Writes MIPS assembly to output stream.
    */
    virtual void compile(std::ostream &assemblyOut) {
        throw std::runtime_error("Not implemented yet by child class.\n");
    }
};
AST::~AST(){
}
