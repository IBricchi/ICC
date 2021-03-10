#include "statement.hpp"

AST_Return::AST_Return(AST* _expr) :
    expr(_expr)
{}

void AST_Return::generateFrames(Frame* _frame){
    frame = _frame;
    copySpecialParamsTo(expr);
    expr->generateFrames(_frame);
}

AST* AST_Return::deepCopy(){
    AST* new_expr = expr->deepCopy();
    return new AST_Return(new_expr);
}

void AST_Return::compile(std::ostream &assemblyOut) {
    std::string retLab = generateUniqueLabel("return");
    assemblyOut << std::endl << "# start " << retLab << std::endl;


    if (expr == nullptr) {
        // return 0 by default
        assemblyOut << "addiu $v0, $0, $0" << std::endl;
    } else {
        // evaluate expression
        expr->compile(assemblyOut);
        
        // set return register to value on top of stack
        assemblyOut << "lw $v0, 8($sp)" << std::endl;;
        // no need to shift stack pointer since return will end a scope anyway
    }

    // skip through frames between current frame and function frame
    for(int i = 0; i < frame->getDistanceToFun(); i++){
        assemblyOut << "lw $fp, 12($fp)" << std::endl;
    }

    // exit last frame properly
    assemblyOut << "move $sp, $fp" << std::endl;
    assemblyOut << "lw $31, 8($sp)" << std::endl;
    assemblyOut << "lw $fp, 12($sp)" << std::endl;
    assemblyOut << "addiu $sp, $sp, " << frame->getStoreSize() << std::endl;
    
    // jump back to wherever you called the function from
    assemblyOut << "jr $31" << std::endl;
    assemblyOut << "nop" << std::endl;

    assemblyOut << "# end " << retLab << std::endl << std::endl;
}

AST_Return::~AST_Return() {
    delete expr;
}

void AST_Break::generateFrames(Frame* _frame) {
    frame = _frame;
}

AST* AST_Break::deepCopy(){
    return new AST_Break();
}

void AST_Break::compile(std::ostream &assemblyOut) {
    std::string returnLab = generateUniqueLabel("return");
    assemblyOut << std::endl << "# start " << returnLab << std::endl;

    std::string endLoopLabel = frame->getEndLoopLabelName();

    // jumps to the end of a loop
    assemblyOut << "j " << endLoopLabel << std::endl;
    assemblyOut << "nop" << std::endl;

    assemblyOut << "# end " << returnLab << std::endl << std::endl;
}

void AST_Continue::generateFrames(Frame* _frame) {
    frame = _frame;
}

AST* AST_Continue::deepCopy(){
    return new AST_Continue();
}

void AST_Continue::compile(std::ostream &assemblyOut) {
    std::string continueLab = generateUniqueLabel("continue");
    assemblyOut << std::endl << "# start " << continueLab << std::endl;

    std::string startLoopLabel = frame->getStartLoopLabelName();

    // jumps to the begining of a loop
    assemblyOut << "j " << startLoopLabel << std::endl;
    assemblyOut << "nop" << std::endl;

    assemblyOut << "# end " << continueLab << std::endl << std::endl;
}

AST_IfStmt::AST_IfStmt(AST* _cond, AST* _then, AST* _other) :
    cond(_cond),
    then(_then),
    other(_other)
{}

void AST_IfStmt::generateFrames(Frame* _frame){
    frame = _frame;
    copySpecialParamsTo(cond);
    cond->generateFrames(_frame);
    // here we don't need to generate a new frame since that is only required if the statement after the if
    // is a block statement, which will itself handle the new frame generation
    copySpecialParamsTo(then);
    then->generateFrames(_frame);

    if (other != nullptr) {
        copySpecialParamsTo(other);
        other->generateFrames(_frame);
    }
}

AST* AST_IfStmt::deepCopy(){
    AST* new_cond = cond->deepCopy();
    AST* new_then = then->deepCopy();
    AST* new_other = nullptr;
    if(other != nullptr){
        new_other = other->deepCopy();
    }
    return new AST_IfStmt(new_cond, new_then, new_other);
}

void AST_IfStmt::compile(std::ostream &assemblyOut) {
    std::string ifLab = generateUniqueLabel("if");
    assemblyOut << std::endl << "# start " << ifLab << std::endl;

    // compile condition code
    cond->compile(assemblyOut);
    // load result of cond expression into register
    // use $t6 as lower $t registers might be used in other compile functions called on right
    assemblyOut << "lw $t6, 8($sp)" << std::endl;
    assemblyOut << "addiu $sp, $sp, 8" << std::endl;
   
    std::string elseLabel = generateUniqueLabel("elseLabel");
    std::string endLabel = generateUniqueLabel("endLabel");

    // branch if condition is false
    assemblyOut << "beq $t6, $0, " << elseLabel << std::endl;
    assemblyOut << "nop" << std::endl;

    // compile then
    then->compile(assemblyOut);

    // always jump to end after going through if branch
    assemblyOut << "j " << endLabel << std::endl;
    assemblyOut << "nop" << std::endl;

    // set else label position
    assemblyOut << elseLabel << ":" << std::endl;
    if (other != nullptr) {
        // compile other
        other->compile(assemblyOut);
    }

    // set end label position
    assemblyOut << endLabel << ":" << std::endl;
    
    assemblyOut << "# end " << ifLab << std::endl << std::endl;
}

AST_IfStmt::~AST_IfStmt(){
    delete cond;
    delete then;
    if (other != nullptr) {
        delete other;
    } 
}

AST_WhileStmt::AST_WhileStmt(AST* _cond, AST* _body):
    cond(_cond),
    body(_body)
{}

void AST_WhileStmt::generateFrames(Frame* _frame){
    frame = _frame;
    copySpecialParamsTo(cond);
    cond->generateFrames(_frame);
    // we don't need a new frame here for the same reason we don't need one for the if statemnt
    copySpecialParamsTo(body);
    body->generateFrames(_frame);
}

AST* AST_WhileStmt::deepCopy(){
    AST* new_cond = cond->deepCopy();
    AST* new_body = body->deepCopy();
    return new AST_WhileStmt(new_cond, new_body);
}

void AST_WhileStmt::compile(std::ostream &assemblyOut){
    std::string whileLab = generateUniqueLabel("while");
    assemblyOut << std::endl << "# start " << whileLab << std::endl; 

    std::string startLoopLabel = generateUniqueLabel("startLoop");
    std::string endLoopLabel = generateUniqueLabel("endLoop");

    // setup labels for looping in the current frame
    // needed for continue and break statements
    frame->setLoopLabelNames(startLoopLabel, endLoopLabel);

    // set start of loop label position
    assemblyOut << startLoopLabel << ":" << std::endl;

    // load result of condition into register
    cond->compile(assemblyOut);
    assemblyOut << "lw $t6, 8($sp)" << std::endl;
    assemblyOut << "addiu $sp, $sp, 8" << std::endl;

    // branch if condition is false
    assemblyOut << "beq $t6, $0, " << endLoopLabel << std::endl;
    assemblyOut << "nop" << std::endl;

    // compile body
    body->compile(assemblyOut);
    
    // always jump back to start of loop
    assemblyOut << "j " << startLoopLabel << std::endl;
    assemblyOut << "nop" << std::endl;

    // set end of loop label position
    assemblyOut << endLoopLabel << ":" << std::endl;

    // remove loop labels from 
    frame->setLoopLabelNames("", "");

    assemblyOut << "# end " << whileLab << std::endl << std::endl;
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
    if(body != nullptr){
        copySpecialParamsTo(body);
        body->generateFrames(frame);
    }
}

AST* AST_Block::deepCopy(){
    AST* new_body = nullptr;
    if(body != nullptr){
        new_body = body->deepCopy();
    }
    return new AST_Block(new_body);
}

void AST_Block::compile(std::ostream &assemblyOut) {
    std::string blockname = generateUniqueLabel("block");
    assemblyOut << std::endl << "# start " << blockname << std::endl;
    if(frame->isFun) assemblyOut << "# ( funciton block ) " << std::endl;

    if(!frame->isFun){
        // increase size of current frame by required ammount for storing previous state data
        // currently storing only $31, and $fp
        assemblyOut << "addiu $sp, $sp, -" << frame->getStoreSize() << std::endl;
        assemblyOut << "sw $31, 8($sp)" << std::endl;
        assemblyOut << "sw $fp, 12($sp)" << std::endl;
        assemblyOut << "move $fp, $sp" << std::endl;

        // move stack pointer down to allocate space for temporary variables in frame
        assemblyOut << "addiu $sp, $sp, -" << frame->getVarStoreSize() << std::endl;
    }

    if (body != nullptr) {
        body->compile(assemblyOut);
    }

    if(!frame->isFun){
        // move fp back to start of frame and re-instate previous frame
        assemblyOut << "move $sp, $fp" << std::endl;
        assemblyOut << "lw $31, 8($sp)" << std::endl;
        assemblyOut << "lw $fp, 12($sp)" << std::endl;
        assemblyOut << "addiu $sp, $sp, " << frame->getStoreSize() << std::endl;
    }
    
    assemblyOut << "# end " << blockname << std::endl << std::endl;
}

AST_Block::~AST_Block(){
    delete body;
}
