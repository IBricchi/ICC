#include "statement.hpp"

AST_Return::AST_Return(AST* _expr) :
    expr(_expr)
{}

void AST_Return::generateFrames(Frame* _frame){
    frame = _frame;
    expr->generateFrames(_frame);
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

        // functions might be defined in external 'driver' file and hence don't load their return value into 'lastResultMemAddress'
        // functions load result directly into $v0
        if (!dynamic_cast<AST_FunctionCall*>(expr)) {
            // load result of expression into register
            assemblyOut << "lw $t0, " << frame->lastResultMemAddress << "($sp)" << std::endl;

            // return result of expression
            assemblyOut << "move $v0, $t0" << std::endl;
        }
    }

    assemblyOut << "# end " << retLab << std::endl << std::endl;
}

AST_Return::~AST_Return() {
    delete expr;
}

void AST_Break::generateFrames(Frame* _frame) {
    frame = _frame;
}

void AST_Break::compile(std::ostream &assemblyOut) {
    std::string endLoopLabel = frame->getEndLoopLabelName();

    assemblyOut << "j " << endLoopLabel << std::endl;
    assemblyOut << "nop" << std::endl;
}

void AST_Continue::generateFrames(Frame* _frame) {
    frame = _frame;
}

void AST_Continue::compile(std::ostream &assemblyOut) {
    std::string startLoopLabel = frame->getStartLoopLabelName();

    assemblyOut << "j " << startLoopLabel << std::endl;
    assemblyOut << "nop" << std::endl;
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

void AST_IfStmt::compile(std::ostream &assemblyOut) {
    cond->compile(assemblyOut);
    // load result of cond expression into register
    // use $t6 as lower $t registers might be used in other compile functions called on right
    assemblyOut << "lw $t6, " << frame->lastResultMemAddress << "($sp)" << std::endl;
   
    std::string elseLabel = generateUniqueLabel("elseLabel");
    std::string endLabel = generateUniqueLabel("endLabel");

    // branch if condition is false
    assemblyOut << "beq $t6, $0, " << elseLabel << std::endl;
    assemblyOut << "nop" << std::endl;

    // compile then
    then->compile(assemblyOut);
    assemblyOut << "j " << endLabel << std::endl;
    assemblyOut << "nop" << std::endl;

    assemblyOut << elseLabel << ":" << std::endl;
    if (other != nullptr) {
        // compile other
        other->compile(assemblyOut);
    }

    assemblyOut << endLabel << ":" << std::endl;
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

void AST_WhileStmt::compile(std::ostream &assemblyOut){
    std::string startLoopLabel = generateUniqueLabel("startLoop");
    std::string endLoopLabel = generateUniqueLabel("endLoop");

    frame->setLoopLabelNames(startLoopLabel, endLoopLabel);

    assemblyOut << startLoopLabel << ":" << std::endl;

    // load result of condition into register
    cond->compile(assemblyOut);
    assemblyOut << "lw $t6, " << frame->lastResultMemAddress << "($sp)" << std::endl;

    // branch if condition is false
    assemblyOut << "beq $t6, $0, " << endLoopLabel << std::endl;
    assemblyOut << "nop" << std::endl;

    // compile body
    body->compile(assemblyOut);
    assemblyOut << "j " << startLoopLabel << std::endl;
    assemblyOut << "nop" << std::endl;

    assemblyOut << endLoopLabel << ":" << std::endl;

    frame->setLoopLabelNames("", "");
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

void AST_Block::compile(std::ostream &assemblyOut) {
    std::string blockname = generateUniqueLabel("block");
    assemblyOut << std::endl << "# start " << blockname << std::endl;
    // header
    // assemblyOut << ".frame	$fp, " << frame->getFrameSize() << " , $31" << std::endl;
    // assemblyOut << ".mask	0x40000000,-4" << std::endl;
    // assemblyOut << ".fmask	0x00000000,0" << std::endl;
    // assemblyOut << ".set	noreorder" << std::endl;
    // assemblyOut << ".set	nomacro" << std::endl;

    assemblyOut << "addiu $sp, $sp, -" << frame->getFrameSize() << std::endl;
    assemblyOut << "sw $31, " << frame->getFrameSize() - 4 << "($sp)" << std::endl;
    assemblyOut << "sw $fp, " << frame->getFrameSize() - 8 << "($sp)" << std::endl;
    assemblyOut << "move $fp, $sp" << std::endl;
    
    if (body != nullptr) {
        body->compile(assemblyOut);
    }

    assemblyOut << "move $sp, $fp" << std::endl;
    assemblyOut << "lw $31, " << frame->getFrameSize() - 4 << "($sp)" << std::endl;
    assemblyOut << "lw $fp, " << frame->getFrameSize() - 8 << "($sp)" << std::endl;
    assemblyOut << "addiu $sp, $sp, " << frame->getFrameSize() << std::endl;

    // footer
    // assemblyOut << ".set	macro" << std::endl;
    // assemblyOut << ".set	reorder" << std::endl;

    assemblyOut << "# end " << blockname << std::endl << std::endl;
}

AST_Block::~AST_Block(){
    delete body;
}
