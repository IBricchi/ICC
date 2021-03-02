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

AST_FunDeclaration::AST_FunDeclaration(std::string _type, std::string* _name, AST* _body) :
    type(_type),
    name(*_name),
    body(_body)
{}

void AST_FunDeclaration::generateFrames(Frame* _frame){
    frame = _frame;
    // we don't need to generate a new frame here since the block statement that will be the body
    // will handle generating the new frame
    if (body != nullptr) {
        body->generateFrames(_frame);
    } 
}

void AST_FunDeclaration::compile(std::ostream &assemblyOut) {
    if (body != nullptr) {
        // function header
        assemblyOut << ".align  2" << std::endl;
        assemblyOut << ".global " << name << std::endl;
        assemblyOut << ".set	nomips16" << std::endl;
        assemblyOut << ".set	nomicromips" << std::endl;
        assemblyOut << ".ent    " << name << std::endl;
        assemblyOut << ".type   " << name << ", @function" << std::endl;

        // create label
        assemblyOut << name << ":" << std::endl;

        // function header 2
        assemblyOut << ".frame	$fp, " << frame->getFrameSize() << " , $31" << std::endl;
        assemblyOut << ".mask	0x40000000,-4" << std::endl;
        assemblyOut << ".fmask	0x00000000,0" << std::endl;
        assemblyOut << ".set	noreorder" << std::endl;
        assemblyOut << ".set	nomacro" << std::endl;

        // body
        body->compile(assemblyOut);

        // function footer
        assemblyOut << ".set	macro" << std::endl;
        assemblyOut << ".set	reorder" << std::endl;
        assemblyOut << ".end    " << name << std::endl;
        assemblyOut << ".size	" << name << ", .-" << name << std::endl;
    }
}

AST_FunDeclaration::~AST_FunDeclaration() {
    if (body != nullptr) {
        delete body;
    }
}

AST_VarDeclaration::AST_VarDeclaration(std::string _type, std::string* _name, AST* _expr) :
    type(_type),
    name(*_name),
    expr(_expr)
{}

void AST_VarDeclaration::generateFrames(Frame* _frame){
    frame = _frame;
    if(expr != nullptr)
        expr->generateFrames(_frame);
    
    _frame->addVariable(name, 4);
}

void AST_VarDeclaration::compile(std::ostream &assemblyOut) {
    /*
        Need to add variable to current frame and assign a memory address to it.
        Don't yet change the value stored in that memory address. This should be
        handled by 'AST_Assignment'.
    */
    frame->addVariable(name, getTypeByteSize(type));
}

AST_VarDeclaration::~AST_VarDeclaration() {
    if(expr != nullptr)
        delete expr; 
}