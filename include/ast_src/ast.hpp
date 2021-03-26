#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <map>
#include <stdexcept>

class Frame;

/*
    Base class for all ast nodes
*/
class AST
{
public:
    Frame* frame;
    bool isVar = false;
    bool returnPtr = false;

    virtual ~AST();
    
    /*
        Generates frames and creates context for them
    */
    virtual void generateFrames(Frame* _frame = nullptr);

    /*
        Writes MIPS assembly to output stream.
    */
    virtual void compile(std::ostream &assemblyOut);

    // overriden by AST_Variable
    virtual void updateVariable(std::ostream &assemblyOut, Frame* currentFrame, std::string reg);

    /*
        This function is only required for source translation of things like short hand assignements
        Deep copy does not currently copy:
        * Frame
    */
    virtual AST* deepCopy();

    virtual void setType(std::string newType);

    /*
        These function is required whenever the type of a node is needed

        Only implemented by Expressions and children of expressions
        (constants, variables, operators, etc.)
    */
    virtual AST* getType();
    virtual int getBytes();

    virtual std::string getTypeName();

    virtual std::string getName();

    virtual void setStructName(std::string newName);
    virtual std::string getStructName();

    virtual int getSize();

    // Used for global variables => cannot get value through stack
    virtual int getIntValue();
    virtual float getFloatValue();
    virtual double getDoubleValue();
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
    std::unordered_map<std::string, AST*> variableType;
    std::unordered_map<std::string, AST*> functions;

    // information about how much memory is needed to preserve previous stack
    // currently only stores state of $fp and $31
    int storeSize = 16;
    
    // information about current memory occupied by variables
    int memOcc = 0;

    /*
        Must be a normal map to preserve ordering.
        Especially important to guarantee that default only appears at end.
        
        Used by switch statement.
        Case children must make labels known to parent switch.

        Default case is special: Its label ends with "default", which can
        be used to identify it.
    */
    std::map<std::string, int> caseLabelValueMapping;

public:
    /*
        Pointer to the parent frame.
        Enables us to access variables from parent frame if they have not been defined locally.
    */
    Frame *parentFrame;

    /*
        Used for global variables.
        There should only ever be one global frame.
    */
    bool isGlobal = false;

    Frame(Frame* _parentFrame = nullptr);

    ~Frame();

    /*
        First tries to find variable in current frame.
        If it does not exist in the current frame, it tries to find it in the parent frame
        and so on.
        
        Returns {-1,-1} if the global frame is reached.
    */
    int getVarPos(const std::string& variableName) const;
    std::pair<int, int> getVarAddress(const std::string &variableName);
    AST* getVarType(const std::string& variableName) const;

    /*
        Does not check if variable already exists.
        If the variable name already exists, it will be overriden.
    */
    void addVariable(const std::string &variableName, AST* type, int byteSize);

    /*
        Same as add variable but for functions
    */
    void addFunction(const std::string &name, AST* fn);
    AST* getFunction(const std::string& name);

    /*
        Used for moving '$sp' pointer when creating new stack frame.
        Is how much memory is required to preserve previous frames state.

        Stack frame must be doubleword (8 byte) aligned (MIPS ABI).
    */
    int getStoreSize() const;

    /*
        Used to set stack pointer in a new frame.
        Is how much memory is required to contain all local variables
    */
    int getVarStoreSize() const;

    void setLoopLabelNames(std::string _startLoopLabelName, std::string _endLoopLabelName);

    void addCaseLabelValueMapping(std::string label, int value);
    std::map<std::string, int> getCaseLabelValueMapping() const;

    /*
        Used for 'return'

        Functions set this flag to true, so when a return statement is called,
        it can calculate exactly how many frames are between it and the function
        it is returning a value for

        Expects there to be a function
        Does not do error checking
    */
    AST* fn;
    std::pair<int, AST*> getFnInfo();

    /* 
        Used for 'break' and 'continue'.

        Enough for them to be strings as should never have more than one active loop
        in one frame (Every block created as new frame).

        Must set these to "" after a loop is finished!
    */
    std::string startLoopLabelName;
    std::string endLoopLabelName;

    /*
        First checks current frame, then parent frame.
        Does not do error checking.

        first element is label name.
        second element is distance to loop frame that contains this label.
    */
    std::pair<std::string, int> getStartLoopLabelName(std::ostream &assemblyOut = std::cout);
    std::pair<std::string, int> getEndLoopLabelName(std::ostream &assemblyOut = std::cout);
};

// Need to come at end of file as dependent on declarations above
#include "util.hpp"
#include "primitive.hpp"
