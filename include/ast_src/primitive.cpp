#include "primitive.hpp"

AST_ConstInt::AST_ConstInt(int _value):
    value(_value)
{}

void AST_ConstInt::generateFrames(Frame* _frame){
    frame = _frame;
}

AST* AST_ConstInt::deepCopy(){
    return new AST_ConstInt(value);
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

AST* AST_Variable::deepCopy(){
    return new AST_Variable(&name);
}

void AST_Variable::compile(std::ostream &assemblyOut) {
    assemblyOut << std::endl << "# start variable read " << name << std::endl;

    // if left of assign load address otherwise load value
    if(specialParams[(int)SpecialParam::LEFT_OF_ASSIGN]){
        assemblyOut << "# (reading address)" << std::endl;
        varAddressToReg(assemblyOut, frame, "$t0", name);
    }
    else{   
        assemblyOut << "# (reading value)" << std::endl;
        varToReg(assemblyOut, frame, "$t0", name);
    }

    // store value in memory
    assemblyOut << "sw $t0, 0($sp)" << std::endl;
    assemblyOut << "addiu $sp, $sp, -8" << std::endl;

    assemblyOut << "# end variable read " << name << std::endl << std::endl;
}

AST* AST_Variable::getType(){
    return frame->getVarType(name);
}

int AST_Variable::getBytes(){
    return getType()->getBytes();
}

void AST_Variable::updateVariable(std::ostream &assemblyOut, Frame* currentFrame, std::string reg) {
    assemblyOut << std::endl << "# start var update " << name << std::endl;

    regToVar(assemblyOut, currentFrame, reg, name);
    
    assemblyOut << "# end var update " << name << std::endl << std::endl;
}

AST_Type::AST_Type(std::string* _name) :
    name(*_name)
{
    bytes = size_of_type[*_name];
}

std::unordered_map<std::string, int> AST_Type::size_of_type = {
    {"int", 4},
    {"char", 4},
    {"float", 4},
    {"double", 8}
};

void AST_Type::generateFrames(Frame* _frame){
    frame = _frame;
}

AST* AST_Type::deepCopy(){
    return new AST_Type(&name);
}

void AST_Type::compile(std::ostream &assemblyOut) {
    throw std::runtime_error("Type should never be compiled.\n");
}

int AST_Type::getBytes(){
    return bytes;
}

AST_ArrayType::AST_ArrayType(AST* _type, int _size) :
    type(_type),
    size(_size)
{
    bytes = _type->getBytes() * size;
}

void AST_ArrayType::generateFrames(Frame* _frame){
    frame = _frame;
    type->generateFrames(_frame);
}

AST* AST_ArrayType::deepCopy(){
    AST* new_type = type->deepCopy();
    return new AST_ArrayType(new_type, size);
}

void AST_ArrayType::compile(std::ostream &assemblyOut) {
    throw std::runtime_error("ArrayType should never be compiled.\n");
}

AST* AST_ArrayType::getType(){
    return type;
}

int AST_ArrayType::getBytes(){
    return bytes;
}

AST_ArrayType::~AST_ArrayType(){
    delete type;
}