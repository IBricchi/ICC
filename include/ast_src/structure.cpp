#include "structure.hpp"

AST_Sequence::AST_Sequence(AST* _first, AST* _second) :
    first(_first),
    second(_second)
{}

void AST_Sequence::compile(std::ostream &assemblyOut, Frame &frame) {
    assemblyOut << first->compile(assemblyOut, frame);
    assemblyOut << endl;

    assemblyOut << second->compile(assemblyOut, frame);
    assemblyOut << endl;
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

void AST_FunDeclaration::compile(std::ostream &assemblyOut, Frame &frame) {
    throw std::runtime_error("Not Implemented Yet.\n");
}

AST_FunDeclaration::~AST_FunDeclaration() {
    delete body;
}

AST_VarDeclaration::AST_VarDeclaration(std::string _type, AST* _assignment) :
    type(_type),
    assignment(_assignment)
{}

void AST_VarDeclaration::compile(std::ostream &assemblyOut, Frame &frame) {
    throw std::runtime_error("Not Implemented Yet.\n");
}

AST_VarDeclaration::~AST_VarDeclaration() {
    delete assignment; 
}
