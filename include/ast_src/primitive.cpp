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

    std::pair<int, int> varAddress = frame->getVarAddress(name);

    // coppy frame pointer to t1 and recurse back expected number of frames
    assemblyOut << "move $t1, $fp" << std::endl;
    for(int i = 0; i < varAddress.first; i++){
        assemblyOut << "lw $t1, 12($t1)" << std::endl;
    }

    // load variable value into register
    assemblyOut << "lw $t0, -" << varAddress.second << "($t1)" << std::endl;

    // store value in memory
    assemblyOut << "sw $t0, 0($sp)" << std::endl;
    assemblyOut << "addiu $sp, $sp, -8" << std::endl;

    assemblyOut << "# end variable read " << name << std::endl << std::endl;
}
