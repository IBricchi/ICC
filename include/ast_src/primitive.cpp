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

AST* AST_ConstInt::getType() {
    std::string typeName = "int";
    return new AST_Type(&typeName);
}

std::string AST_ConstInt::getTypeName(){
    return "int";
}

int AST_ConstInt::getIntValue() {
    return value;
}

AST_ConstFloat::AST_ConstFloat(float _value):
    value(_value)
{}

void AST_ConstFloat::generateFrames(Frame* _frame){
    frame = _frame;
}

AST* AST_ConstFloat::deepCopy(){
    return new AST_ConstFloat(value);
}

void AST_ConstFloat::compile(std::ostream &assemblyOut){
    assemblyOut << std::endl << "# start const float " << value << std::endl;

    // load constant into register
    assemblyOut << "li.s $f4, " << value << std::endl;

    // store constant to top of stack
    assemblyOut << "s.s $f4, 0($sp)" << std::endl;
    assemblyOut << "addiu $sp, $sp, -8" << std::endl;

    assemblyOut << "# end const float " << value << std::endl << std::endl;
}

AST* AST_ConstFloat::getType() {
    std::string typeName = "float";
    return new AST_Type(&typeName);
}

std::string AST_ConstFloat::getTypeName(){
    return "float";
}

float AST_ConstFloat::getFloatValue() {
    return value;
}

AST_ConstDouble::AST_ConstDouble(double _value):
    value(_value)
{}

void AST_ConstDouble::generateFrames(Frame* _frame){
    frame = _frame;
}

AST* AST_ConstDouble::deepCopy(){
    return new AST_ConstDouble(value);
}

void AST_ConstDouble::compile(std::ostream &assemblyOut){
    assemblyOut << std::endl << "# start const double " << value << std::endl;

    // load constant into register
    assemblyOut << "li.d $f4, " << value << std::endl;

    // store constant to top of stack
    assemblyOut << "s.d $f4, 0($sp)" << std::endl;
    assemblyOut << "addiu $sp, $sp, -8" << std::endl;

    assemblyOut << "# end const double " << value << std::endl << std::endl;
}

AST* AST_ConstDouble::getType() {
    std::string typeName = "double";
    return new AST_Type(&typeName);
}

std::string AST_ConstDouble::getTypeName(){
    return "double";
}

double AST_ConstDouble::getDoubleValue() {
    return value;
}

AST_ConstChar::AST_ConstChar(char _value):
    value(_value)
{}

void AST_ConstChar::generateFrames(Frame* _frame){
    frame = _frame;
}

AST* AST_ConstChar::deepCopy(){
    return new AST_ConstChar(value);
}

void AST_ConstChar::compile(std::ostream &assemblyOut){
    assemblyOut << std::endl << "# start const char " << value << " (" << (int)value << ")" << std::endl;
    
    // load constant into register
    assemblyOut << "li $t0, " << (int)value << std::endl;

    // store constant to top of stack
    assemblyOut << "sw $t0, 0($sp)" << std::endl;
    assemblyOut << "addiu $sp, $sp, -8" << std::endl;

    assemblyOut << "# end const char " << value << " (" << (int)value << ")" << std::endl << std::endl;
}

AST* AST_ConstChar::getType() {
    std::string typeName = "char";
    return new AST_Type(&typeName);
}

int AST_ConstChar::getIntValue() {
    return value;
}

AST_Variable::AST_Variable(std::string* _name) :
    name(*_name)
{
    isVar = true;
}

void AST_Variable::generateFrames(Frame* _frame){
    frame = _frame;
}

AST* AST_Variable::deepCopy(){
    return new AST_Variable(&name);
}

void AST_Variable::compile(std::ostream &assemblyOut) {
    std::string varType = this->getType()->getTypeName();

    assemblyOut << std::endl << "# start " << varType << " variable read " << name << std::endl;

    // if left of assign load address otherwise load value
    if(returnPtr){
        assemblyOut << "# (reading address)" << std::endl;
        
        varAddressToReg(assemblyOut, frame, "$t0", name);

        // store value in memory
        assemblyOut << "sw $t0, 0($sp)" << std::endl;
    }
    else{   
        assemblyOut << "# (reading value)" << std::endl;

        if (varType == "float") {
            varToReg(assemblyOut, frame, "$f4", name);

            // store value in memory
            assemblyOut << "s.s $f4, 0($sp)" << std::endl;
        } else if (varType == "double") {
            varToReg(assemblyOut, frame, "$f4", name);

            // store value in memory
            assemblyOut << "s.d $f4, 0($sp)" << std::endl;
        } else {
            varToReg(assemblyOut, frame, "$t0", name);

            // store value in memory
            assemblyOut << "sw $t0, 0($sp)" << std::endl;
        }
    }

    assemblyOut << "addiu $sp, $sp, -8" << std::endl;

    assemblyOut << "# end " << varType << " variable read " << name << std::endl << std::endl;
}

AST* AST_Variable::getType(){
    return frame->getVarType(name);
}

int AST_Variable::getBytes(){
    return getType()->getBytes();
}

std::string AST_Variable::getTypeName() {
    return getType()->getTypeName();
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

AST_Type::AST_Type(std::string* _name, const std::map<std::string, std::string> &attributeNameTypeMap) :
     name(*_name)
{
    bytes = 0;
    for (auto attribute : attributeNameTypeMap) {
        if (attribute.second == "struct") {
            bytes += frame->getVarType(attribute.first)->getBytes();
        } else if (attribute.second.find("*") != std::string::npos) {
            // array
            std::string typeName = attribute.second.substr(0, attribute.second.find("*"));
            int size = std::stoi(attribute.second.substr(attribute.second.find("*")+1));
            bytes += size_of_type[typeName] * size;
        } else if (attribute.second == "char") {
            // size_of_type map contains incorrect char size
            bytes += 1;
        } else {
            bytes += size_of_type[attribute.second];
        }
    }
}

std::unordered_map<std::string, int> AST_Type::size_of_type = {
    {"int", 4}, // Intentionally wrong so that char can be treated as int for binary/unary operations (e.g. using lw instead of lb)
    {"char", 4},
    {"float", 4},
    {"double", 8},
    {"unsigned", 4},
    {"struct", -1}, // Size needs to be computed dynamically
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

std::string AST_Type::getTypeName() {
    return name;
}

AST_ArrayType::AST_ArrayType(AST* _type, int _size) :
    type(_type),
    size(_size)
{
    if (_type->getTypeName() == "char") {
        bytes = size;
    } else {
        bytes = _type->getBytes() * size;
    }
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

std::string AST_ArrayType::getTypeName(){
    return "pointer";
}

int AST_ArrayType::getSize() {
    return this->size;
}

AST_ArrayType::~AST_ArrayType(){
    delete type;
}

AST_Pointer::AST_Pointer(AST* _type) :
    type(_type)
{}

void AST_Pointer::generateFrames(Frame* _frame){
    frame = _frame;
    type->generateFrames(_frame);
}

AST* AST_Pointer::deepCopy(){
    AST* new_type = type->deepCopy();
    return new AST_Pointer(new_type);
}

void AST_Pointer::compile(std::ostream &assemblyOut) {
    throw std::runtime_error("PointerType should never be compiled.\n");
}

AST* AST_Pointer::getType(){
    return type;
}

int AST_Pointer::getBytes(){
    return 4;
}

std::string AST_Pointer::getTypeName(){
    return "pointer";
}

AST_Pointer::~AST_Pointer(){
    delete type;
}

void AST_NoEffect::generateFrames(Frame* _frame) {
     frame = _frame;
}

void AST_NoEffect::compile(std::ostream &assemblyOut) {
    // Do nothing 
}
