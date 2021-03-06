#include "statement.hpp"

AST_Return::AST_Return(AST* _expr) :
    expr(_expr)
{}

void AST_Return::generateFrames(Frame* _frame){
    frame = _frame;
    expr->generateFrames(_frame);
}

AST* AST_Return::deepCopy(){
    AST* new_expr = expr->deepCopy();
    return new AST_Return(new_expr);
}

void AST_Return::compile(std::ostream &assemblyOut) {
    std::string retLab = generateUniqueLabel("return");
    assemblyOut << std::endl << "# start " << retLab << std::endl;

    // get info on corresponding function
    std::pair<int, AST*> fnInfo = frame->getFnInfo();

    if (expr == nullptr) {
        // return 0 by default
        assemblyOut << "addiu $v0, $0, $0" << std::endl;
    } else {
        // evaluate expression
        expr->compile(assemblyOut);
        
        // set return register to value on top of stack
        std::string fnTypeName = fnInfo.second->getTypeName();
        if(fnTypeName == "float")
            assemblyOut << "l.s $f0, 8($sp)" << std::endl;
        else if(fnTypeName == "double")
            assemblyOut << "l.d $f0, 8($sp)" << std::endl;
        else
            assemblyOut << "lw $v0, 8($sp)" << std::endl;;
        // no need to shift stack pointer since return will end a scope anyway
    }

    // skip through frames between current frame and function frame
    for(int i = 0; i < fnInfo.first; i++){
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
    std::string breakLabel = generateUniqueLabel("break");
    assemblyOut << std::endl << "# start " << breakLabel << std::endl;

    auto endLoopLabel = frame->getEndLoopLabelName();

    // skip through frames between current frame and loop frame
    for(int i = 0; i < endLoopLabel.second - 1 ; i++){
        assemblyOut << "lw $fp, 12($fp)" << std::endl;
    }

    // exit last frame properly
    if (endLoopLabel.second != 0) {
        assemblyOut << "move $sp, $fp" << std::endl;
        assemblyOut << "lw $31, 8($sp)" << std::endl;
        assemblyOut << "lw $fp, 12($sp)" << std::endl;
        assemblyOut << "addiu $sp, $sp, " << frame->getStoreSize() << std::endl;
    }

    // jumps to the end of a loop
    assemblyOut << "j " << endLoopLabel.first << std::endl;
    assemblyOut << "nop" << std::endl;

    assemblyOut << "# end " << breakLabel << std::endl << std::endl;
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

    auto startLoopLabel = frame->getStartLoopLabelName();

    // skip through frames between current frame and loop frame
    for(int i = 0; i < startLoopLabel.second - 1 ; i++){
        assemblyOut << "lw $fp, 12($fp)" << std::endl;
    }

    // exit last frame properly
    if (startLoopLabel.second != 0) {
         assemblyOut << "move $sp, $fp" << std::endl;
        assemblyOut << "lw $31, 8($sp)" << std::endl;
        assemblyOut << "lw $fp, 12($sp)" << std::endl;
        assemblyOut << "addiu $sp, $sp, " << frame->getStoreSize() << std::endl;
    }

    // jumps to the begining of a loop
    assemblyOut << "j " << startLoopLabel.first << std::endl;
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
    cond->generateFrames(_frame);
    // here we don't need to generate a new frame since that is only required if the statement after the if
    // is a block statement, which will itself handle the new frame generation
    then->generateFrames(_frame);

    if (other != nullptr) {
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
    cond->generateFrames(_frame);
    // we don't need a new frame here for the same reason we don't need one for the if statemnt
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

AST_SwitchStmt::AST_SwitchStmt(AST* _value, AST* _body):
    value(_value),
    body(_body)
{}

void AST_SwitchStmt::generateFrames(Frame* _frame){
    frame = _frame;
    value->generateFrames(_frame);
    body->generateFrames(_frame);
}

void AST_SwitchStmt::compile(std::ostream &assemblyOut){
    std::string switchStmt = generateUniqueLabel("switchStmt");
    assemblyOut << std::endl << "# start " << switchStmt << std::endl;

    // needed for break statements
    std::string endSwitchLabel = generateUniqueLabel("endSwitch");
    frame->setLoopLabelNames("", endSwitchLabel);

    value->compile(assemblyOut);

    // load top of stack into register
    assemblyOut << "lw $t4, 8($sp)" << std::endl;
    assemblyOut << "addiu $sp, $sp, 8" << std::endl;

    // Every switch statement will have exactly one block that encapsulates its cases (body of switch).
    // The below logic will jump into one of these cases and while doing so jump over the
    // beginning of the block. The beginning of the block is normally responsible for opening
    // a new frame.
    // Therefore, we must open this new frame manually to counter this problem. The frame will be closed
    // by the end of block as normal.
    assemblyOut << "addiu $sp, $sp, -" << body->frame->getStoreSize() << std::endl;
    assemblyOut << "sw $31, 8($sp)" << std::endl;
    assemblyOut << "sw $fp, 12($sp)" << std::endl;
    assemblyOut << "move $fp, $sp" << std::endl;
    assemblyOut << "addiu $sp, $sp, -" << body->frame->getVarStoreSize() << std::endl;

    auto caseLabelToValueMapping = frame->getCaseLabelValueMapping();
    for (const auto &labelValue : caseLabelToValueMapping) {
        if (hasEnding(labelValue.first, "default") == true) {
            assemblyOut << "j " << labelValue.first << std::endl;
            assemblyOut << "nop" << std::endl;
        } else {
            assemblyOut << "li $t5, " << labelValue.second << std::endl;    

            assemblyOut << "beq $t4, $t5, " << labelValue.first << std::endl;
            assemblyOut << "nop" << std::endl;
        }
    }

    // case statements
    body->compile(assemblyOut);

    assemblyOut << endSwitchLabel << ":" << std::endl;

    // remove loop labels from 
    frame->setLoopLabelNames("", "");

    assemblyOut << "# end " << switchStmt << std::endl; 
}

AST_SwitchStmt::~AST_SwitchStmt(){
    delete value;
    delete body;
}

AST_CaseStmt::AST_CaseStmt(AST* _body, int _value):
    body(_body),
    value(_value),
    isDefaultCase(false)
{}

AST_CaseStmt::AST_CaseStmt(AST* _body):
    body(_body),
    isDefaultCase(true)
{}

void AST_CaseStmt::generateFrames(Frame* _frame){
    frame = _frame;

    // make accessible to parent AST_SwitchStmt
    if (!isDefaultCase) {
        caseStartLabel = generateUniqueLabel("caseStmt") + "_" + std::to_string(value);
        frame->parentFrame->addCaseLabelValueMapping(caseStartLabel, value);
    } else {
        caseStartLabel = generateUniqueLabel("caseStmt") + "_default";
        frame->parentFrame->addCaseLabelValueMapping(caseStartLabel, 0);
    }
    

    body->generateFrames(_frame);
}

void AST_CaseStmt::compile(std::ostream &assemblyOut){
    std::string caseStmt = generateUniqueLabel("caseStmt");
    assemblyOut << std::endl << "# start " << caseStmt << std::endl;

    assemblyOut << caseStartLabel << ":" << std::endl;

    body->compile(assemblyOut);

    assemblyOut << "# end " << caseStmt << std::endl; 
}

AST_CaseStmt::~AST_CaseStmt(){
    delete body;
}

AST_Block::AST_Block(AST* _body):
    body(_body)
{}

void AST_Block::generateFrames(Frame* _frame){
    // here we creat a new frame since blocks generate new scopes
    frame = new Frame(_frame);
    if(body != nullptr){
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
    if(frame->fn != nullptr) assemblyOut << "# ( funciton block ) " << std::endl;

    else{
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

    if(frame->fn == nullptr){
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
