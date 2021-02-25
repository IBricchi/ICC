#include "expression.hpp"

AST_VarAssign::AST_VarAssign(std::string* _name, AST* _expr):
    name(*_name),
    expr(_expr)
{}

void AST_VarAssign::generateFrames(Frame* _frame){
    frame = _frame;
    expr->generateFrames(_frame);
}

void AST_VarAssign::compile(std::ostream &assemblyOut){
    throw std::runtime_error("AST_VarAssign: Not Implemented Yet.\n");
}

AST_VarAssign::~AST_VarAssign(){
    delete expr;
}

AST_FunctionCall::AST_FunctionCall(std::string* _functionName):
    functionName(*_functionName)
{
    args = {};
    parity = 0;
}

template <class ...TArgs>
AST_FunctionCall::AST_FunctionCall(std::string* _functionName, TArgs... _args):
    functionName(*_functionName),
    args{_args...}
{
    parity = args.size();
}

void AST_FunctionCall::generateFrames(Frame* _frame){
    frame = _frame;
}

void AST_FunctionCall::compile(std::ostream &assemblyOut) {
    throw std::runtime_error("AST_FunctionCall: Not Implemented Yet.\n");
}

AST_FunctionCall::~AST_FunctionCall() {
    // delete name;
}

AST_BinOp::AST_BinOp(AST_BinOp::Type _type, AST* _left, AST* _right):
    type(_type),
    left(_left),
    right(_right)
{}

void AST_BinOp::generateFrames(Frame* _frame){
    frame = _frame;
    left->generateFrames(_frame);
    right->generateFrames(_frame);
}

void AST_BinOp::compile(std::ostream &assemblyOut) {
    throw std::runtime_error("AST_BinOp: Not Implemented Yet.\n");
}

AST_BinOp::~AST_BinOp(){
    delete left;
    delete right;
}

AST_UnOp::AST_UnOp(AST_UnOp::Type _type, AST* _operand):
    type(_type),
    operand(_operand)
{}

void AST_UnOp::generateFrames(Frame* _frame){
    frame = _frame;
    operand->generateFrames(_frame);
}

void AST_UnOp::compile(std::ostream &assemblyOut) {
    throw std::runtime_error("AST_UnOp: Not Implemented Yet.\n");
}

AST_UnOp::~AST_UnOp(){
    delete operand;
}
