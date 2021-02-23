#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <stdexcept>

class Frame;

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
    virtual void compile(std::ostream &assemblyOut, Frame &frame); 
};

/*
    Class that contains frame information.
    A new Frame object is created for each frame.
*/
class Frame
{
protected:
    /* 
        map of variable names to memory address relative to frame pointer
        retrieve using 'lw ${destinationReg} {variableBindings[variableName]}($fp)'
    */ 
    unordered_map<std::string, int> variableBindings;

    /*
        Pointer to the parent frame.
        Enables us to access variables from parent frame if they have not been defined locally.
    */
    Frame *parentFrame;

public:
    Frame(Frame _parentFrame = nullptr);

    ~Frame();

    /*
        First tries to find variable in current frame.
        If it does not exist in the current frame, it tries to find it in the parent frame
        and so on.
        Expects the variable to exist in its current frame or one of its parent frame:
        Does not do error checking.
    */
    int getMemoryAddress(const string &variableName);

    /*
        Does not check if variable already exists.
        If the variable name already exists, it will be overriden.
    */
    void addVariable(string variableName, int memAddress);
};
