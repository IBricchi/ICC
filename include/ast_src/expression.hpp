#pragma once

#include "ast.hpp"

class AST_FunctionCall
    : public AST
{
private:
    AST* name;
    /*
        Need some data structure for arguments:

        - Arguments can be constants, variables, expressions, etc.
        - Can have 0 to n arguments.
        - Arguments might have a default value (optional arguments).
        - Their position in the argument list has a meaning depending on the corresponding 
            position in the function declaration.
    */

public:
    AST_FunctionCall(AST* _name);

    void compile(std::ostream &assemblyOut, Frame &frame) override;

    ~AST_FunctionCall();
}
