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
    expr->compile(assemblyOut);

    // functions might be defined in external 'driver' file and hence don't load their return value into 'lastResultMemAddress'
    // functions load result directly into $v0
    if (!dynamic_cast<AST_FunctionCall*>(expr)) {
        // load result of expression into register
        assemblyOut << "lw $t0, " << frame->lastResultMemAddress << "($sp)" << std::endl;
    } else {
        // load function call result into register
        assemblyOut << "move $t0, $v0" << std::endl;
    }

    // store register data into variable's memory address
    assemblyOut << "sw $t0, " << frame->getMemoryAddress(name) << "($sp)" << std::endl;
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
    for (AST* arg : args) {
        // ...
        throw std::runtime_error("AST_FunctionCall: Not Implemented For Arguments Yet.\n");
    }

    assemblyOut << "jal " << functionName << std::endl;
    assemblyOut << "nop" << std::endl;
}

AST_FunctionCall::~AST_FunctionCall() {
    for (AST* arg : args) {
        if (arg != nullptr) {
            delete arg;
        }
    }
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
