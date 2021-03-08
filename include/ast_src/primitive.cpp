#include "primitive.hpp"

AST_ConstInt::AST_ConstInt(int _value):
    value(_value)
{}

void AST_ConstInt::generateFrames(Frame* _frame){
    frame = _frame;
}

void AST_ConstInt::compile(std::ostream &assemblyOut){
    assemblyOut << std::endl << "# start const int " << value << std::endl;
    
    // load constant into register
    assemblyOut << "addiu $t0, $0, " << value << std::endl;

    // store constant to top of stack
    assemblyOut << "sw $t0, 0($sp)" << std::endl;
    assemblyOut << "addiu $sp, $sp, -8" << std::endl;

    assemblyOut << "# end const int " << value << std::endl << std::endl;
}

AST_Variable::AST_Variable(std::string* _name) :
    name(*_name)
{}

void AST_Variable::generateFrames(Frame* _frame){
    frame = _frame;
}

void AST_Variable::compile(std::ostream &assemblyOut) {
    assemblyOut << std::endl << "# start variable read " << name << std::endl;

    varToReg(assemblyOut, frame, "$t0", name);

    // store value in memory
    assemblyOut << "sw $t0, 0($sp)" << std::endl;
    assemblyOut << "addiu $sp, $sp, -8" << std::endl;

    assemblyOut << "# end variable read " << name << std::endl << std::endl;
}

void AST_Variable::updateVariable(std::ostream &assemblyOut, Frame* currentFrame, std::string reg) {
    assemblyOut << std::endl << "# start var update " << name << std::endl;

    regToVar(assemblyOut, currentFrame, reg, name);
    
    assemblyOut << "# end var update " << name << std::endl << std::endl;
}
