#include "expression.hpp"

AST_FunctionCall::AST_FunctionCall(string* _functionName) :
    functionName(_functionName)
{}

void AST_FunctionCall::compile(std::ostream &assemblyOut, Frame &frame) {
    /*
        need to create new Frame object and initialise it with the current frame object
        as the parent frame
    */

    throw std::runtime_error("Not Implemented Yet.\n");
}

AST_FunctionCall::~AST_FunctionCall() {
    delete name;
}
