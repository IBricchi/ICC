#include "primitive.hpp"

AST_ConstInt::AST_ConstInt(int _value):
    value(_value)
{}

void AST_ConstInt::generateFrames(Frame* _frame){
    frame = _frame;
}

void AST_ConstInt::compile(std::ostream &assemblyOut){
    // load constant into register
    assemblyOut << "addiu $t0, $0, " << value << std::endl;

    // store constant in memory
    int relativeMemAddress = frame->getFrameSize() - frame->getMemOcc() - 5*4;
    frame->lastResultMemAddress = relativeMemAddress;
    assemblyOut << "sw $t0, " << relativeMemAddress << "($sp)" << std::endl;
}

AST_Variable::AST_Variable(std::string* _name) :
    name(*_name)
{}

void AST_Variable::generateFrames(Frame* _frame){
    frame = _frame;
}

void AST_Variable::compile(std::ostream &assemblyOut) {
    // load variable value into register
    assemblyOut << "lw $t0, " << frame->getMemoryAddress(name) << "($sp)" << std::endl;

    // store value in memory
    int relativeMemAddress = frame->getFrameSize() - frame->getMemOcc() - 5*4;
    frame->lastResultMemAddress = relativeMemAddress;
    assemblyOut << "sw $t0, " << relativeMemAddress << "($sp)" << std::endl;
}