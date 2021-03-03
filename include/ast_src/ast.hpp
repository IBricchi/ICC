#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <stdexcept>

#include "util.hpp"

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
private:
    /* 
        map of variable names to memory address relative to frame pointer
        retrieve using 'lw ${destinationReg} {variableBindings[variableName]}($fp)'
    */ 
    std::unordered_map<std::string, int> variableBindings;

    // information about current memory occupied by variables
    // starts at a constant due to needing a minimum ammount of space in memeory
    // for each frame
    int memOcc = 16;

    /*
        Pointer to the parent frame.
        Enables us to access variables from parent frame if they have not been defined locally.
    */
    Frame *parentFrame;

public:
    /* 
        Memory address where the last result is stored relative to the stack pointer.
        For array, this points to beginning of the array.
    */
    int lastResultMemAddress;

    Frame(Frame* _parentFrame = nullptr);

    ~Frame();

    /*
        First tries to find variable in current frame.
        If it does not exist in the current frame, it tries to find it in the parent frame
        and so on.
        Expects the variable to exist in its current frame or one of its parent frame:
        Does not do error checking.
    */
    int getMemoryAddress(const std::string &variableName) const;

    /*
        Does not check if variable already exists.
        If the variable name already exists, it will be overriden.
    */
    void addVariable(const std::string &variableName, int byteSize);

    /*
        Used for moving '$sp' pointer when creating new stack frame.

        Stack frame must be doubleword (8 byte) aligned (MIPS ABI).
    */
    int getFrameSize() const;

    int getMemOcc() const;
};
