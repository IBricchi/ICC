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
    Frame* frame;

public:
    virtual ~AST();
    
    /*
        Generates frames and creates context for them
    */
    virtual void generateFrames(Frame* _frame = nullptr);
    
    /*
        Writes MIPS assembly to output stream.
    */
    virtual void compile(std::ostream &assemblyOut);
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
    std::unordered_map<std::string, int> variableBindings;

    /*
        information about current memory occupied by variables
    */
   int memOcc = 0;

    /*
        Pointer to the parent frame.
        Enables us to access variables from parent frame if they have not been defined locally.
    */
    Frame *parentFrame;

public:
    Frame(Frame* _parentFrame = nullptr);

    ~Frame();

    /*
        First tries to find variable in current frame.
        If it does not exist in the current frame, it tries to find it in the parent frame
        and so on.
        Expects the variable to exist in its current frame or one of its parent frame:
        Does not do error checking.
    */
    int getMemoryAddress(const std::string &variableName);

    /*
        Does not check if variable already exists.
        If the variable name already exists, it will be overriden.
    */
    void addVariable(const std::string &variableName, int byteSize);

    /*
        Used for moving '$sp' pointer when creating new stack frame.

        Stack frame must be doubleword (8 byte) aligned (MIPS ABI).
    */
    int getFrameSize();
};
