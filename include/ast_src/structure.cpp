#include "structure.hpp"

AST_Sequence::AST_Sequence(AST* _first, AST* _second) :
    first(_first),
    second(_second)
{}

void AST_Sequence::generateFrames(Frame* _frame){
    frame = _frame;
    first->generateFrames(_frame);
    second->generateFrames(_frame);
}

void AST_Sequence::compile(std::ostream &assemblyOut) {
    first->compile(assemblyOut);
    second->compile(assemblyOut);
}

AST_Sequence::~AST_Sequence(){
    delete first;
    delete second;
}

AST_FunDeclaration::AST_FunDeclaration(std::string _type, std::string _name, AST* _body) :
    type(_type),
    name(_name),
    body(_body)
{}

void AST_FunDeclaration::generateFrames(Frame* _frame){
    frame = _frame;
    body->generateFrames(new Frame(_frame));
}

void AST_FunDeclaration::compile(std::ostream &assemblyOut) {
    throw std::runtime_error("Not Implemented Yet.\n");
}

AST_FunDeclaration::~AST_FunDeclaration() {
    delete body;
}

AST_VarDeclaration::AST_VarDeclaration(std::string _type, std::string _name, AST* _expr) :
    type(_type),
    name(_name),
    expr(_expr)
{}

void AST_VarDeclaration::generateFrames(Frame* _frame){
    frame = _frame;
    if(expr != nullptr)
        expr->generateFrames(_frame);
}

void AST_VarDeclaration::compile(std::ostream &assemblyOut) {
    /*
        Need to add variable to current frame and assign a memory address to it.
        Don't yet change the value stored in that memory address. This should be
        handled by 'AST_Assignment'.
    */
    throw std::runtime_error("Not Implemented Yet.\n");
}

AST_VarDeclaration::~AST_VarDeclaration() {
    if(expr != nullptr)
        delete expr; 
}
