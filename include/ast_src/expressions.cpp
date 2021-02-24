#include "expression.hpp"

template <class ...TArgs>
AST_FunctionCall::AST_FunctionCall(std::string _functionName, TArgs... _args):
    functionName(_functionName)
{
    args = _args
    parity = args.size();
}

void AST_FunctionCall::generateFrames(Frame* _frame){
    frame = _frame;
}

void AST_FunctionCall::compile(std::ostream &assemblyOut) {
    throw std::runtime_error("Not Implemented Yet.\n");
}

AST_FunctionCall::~AST_FunctionCall() {
    // delete name;
}
