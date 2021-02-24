#include "statement.hpp"

AST_Return::AST_Return(AST* _expr) :
    expr(_expr)
{}

void AST_Return::generateFrames(Frame* _frame){
    frame = _frame;
    expr->generateFrames(_frame);
}

void AST_Return::compile(std::ostream &assemblyOut) {
    if (expr == nullptr) {
        // return 0 by default
        assemblyOut << "addiu $v0, $0, $0" << std::endl;
    }
    else {
        // Need to first evaluate expression (likely multiple assembly lines)
        // and then somehow put the final value into register $v0.
        // How do we find out what the final value is? Keep track of last register that we stored something in? Some sort of context?
        // Can probably use the same idea as for loading variables into registers and then using that register in future assembly.

        throw std::runtime_error("Not Implemented Yet.\n");
    }

    // Need to do stuff for ending function call.
    // E.g. moving stack and frame pointers
    // See lecture 10 for help
}

AST_Return::~AST_Return() {
    delete expr;
}

AST_IfStmt::AST_IfStmt(AST* _cond, AST* _then, AST* _other) :
    cond(_cond),
    then(_then),
    other(_other)
{}

void AST_IfStmt::generateFrames(Frame* _frame){
    frame = _frame;
    cond->generateFrames(_frame);
    // here we don't need to generate a new frame since that is only required if the statement after the if
    // is a block statement, which will itself handle the new frame generation
    then->generateFrames(_frame);
    other->generateFrames(_frame);
}

void AST_IfStmt::compile(std::ostream &assemblyOut) {
    throw std::runtime_error("Not Implemented Yet.\n");
}

AST_IfStmt::~AST_IfStmt(){
    delete cond;
    delete then;
    delete other;
}

AST_WhileStmt::AST_WhileStmt(AST* _cond, AST* _body):
    cond(_cond),
    body(_body)
{}

void AST_WhileStmt::generateFrames(Frame* _frame){
    frame = _frame;
    cond->generateFrames(_frame);
    // we don't need a new frame here for the same reason we don't need one for the if statemnt
    body->generateFrames(_frame);
}

void AST_WhileStmt::compile(std::ostream &assemblyOut){
    throw std::runtime_error("Note Implemented Yet.\n");
}

AST_WhileStmt::~AST_WhileStmt(){
    delete cond;
    delete body;
}

AST_Block::AST_Block(AST* _body):
    body(_body)
{}

void AST_Block::generateFrames(Frame* _frame){
    // here we creat a new frame since blocks generate new scopes
    frame = new Frame(_frame);
    body->generateFrames(frame);
}

void AST_Block::compile(std::ostream &assemblyOut){
    throw std::runtime_error("Note Implemented Yet.\n");
}

AST_Block::~AST_Block(){
    delete body;
}