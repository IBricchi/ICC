#pragma once

#include "ast.hpp"

class AST_FunctionCall
    : public AST
{
private:
    std::string functionName;
    std::vector<AST*> args;
    int parity; // number of arguments
    /*
        Need some data structure for arguments:

        - Arguments can be constants, variables, expressions, etc.
        - Can have 0 to n arguments.
        - Arguments might have a default value (optional arguments).
        - Their position in the argument list has a meaning depending on the corresponding 
            position in the function declaration.

        I think the best way to do this would just be a vector of AST*
        Where each node can be any of the things you mentioned in your first node
        We'd have to see how to make that work with the parser though I've added a possible constructor
        Which might be useful later
    */

public:
    template<class ...TArgs> 
    AST_FunctionCall(std::string _functionName, TArgs... _params);

    void generateFrames(Frame* _frame = nullptr) override;
    void compile(std::ostream &assemblyOut) override;

    ~AST_FunctionCall();
};
