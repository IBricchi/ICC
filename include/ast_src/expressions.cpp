#include "expression.hpp"

AST_VarAssign::AST_VarAssign(std::string* _name, AST* _expr):
    name(*_name),
    expr(_expr)
{}

void AST_VarAssign::generateFrames(Frame* _frame){
    frame = _frame;
    expr->generateFrames(_frame);
}

void AST_VarAssign::compile(std::ostream &assemblyOut){
    expr->compile(assemblyOut);

    // functions might be defined in external 'driver' file and hence don't load their return value into 'lastResultMemAddress'
    // functions load result directly into $v0
    if (!dynamic_cast<AST_FunctionCall*>(expr)) {
        // load result of expression into register
        assemblyOut << "lw $t0, " << frame->lastResultMemAddress << "($sp)" << std::endl;
    } else {
        // load function call result into register
        assemblyOut << "move $t0, $v0" << std::endl;
    }

    // store register data into variable's memory address
    assemblyOut << "sw $t0, " << frame->getMemoryAddress(name) << "($sp)" << std::endl;
}

AST_VarAssign::~AST_VarAssign(){
    delete expr;
}

AST_FunctionCall::AST_FunctionCall(std::string* _functionName):
    functionName(*_functionName)
{
    args = {};
    parity = 0;
}

template <class ...TArgs>
AST_FunctionCall::AST_FunctionCall(std::string* _functionName, TArgs... _args):
    functionName(*_functionName),
    args{_args...}
{
    parity = args.size();
}

void AST_FunctionCall::generateFrames(Frame* _frame){
    frame = _frame;
}

void AST_FunctionCall::compile(std::ostream &assemblyOut) {
    for (AST* arg : args) {
        // ...
        throw std::runtime_error("AST_FunctionCall: Not Implemented For Arguments Yet.\n");
    }

    assemblyOut << "jal " << functionName << std::endl;
    assemblyOut << "nop" << std::endl;
}

AST_FunctionCall::~AST_FunctionCall() {
    for (AST* arg : args) {
        if (arg != nullptr) {
            delete arg;
        }
    }
}

AST_BinOp::AST_BinOp(AST_BinOp::Type _type, AST* _left, AST* _right):
    type(_type),
    left(_left),
    right(_right)
{}

void AST_BinOp::generateFrames(Frame* _frame){
    frame = _frame;
    left->generateFrames(_frame);
    right->generateFrames(_frame);
}

void AST_BinOp::compile(std::ostream &assemblyOut) {
    left->compile(assemblyOut);
    // load result of left expression into register
    // use $t6 as lower $t registers might be used in other compile functions called on right
    assemblyOut << "lw $t6, " << frame->lastResultMemAddress << "($sp)" << std::endl;

    right->compile(assemblyOut);
    // load result of right expression into register
    assemblyOut << "lw $t1, " << frame->lastResultMemAddress << "($sp)" << std::endl;

    switch (type) {
        case Type::LOGIC_OR:
        {
            std::string trueLabel = generateUniqueLabel("trueLabel");
            std::string falseLabel = generateUniqueLabel("falseLabel");
            std::string endLabel = generateUniqueLabel("end");

            // evaluate first expression first => short-circuit evaluation
            assemblyOut << "bne $t6, $0, " << trueLabel << std::endl;
            assemblyOut << "nop" << std::endl;

            assemblyOut << "bne $t1, $0, " << trueLabel << std::endl;
            assemblyOut << "nop" << std::endl;

            assemblyOut << falseLabel << ":" << std::endl;
            assemblyOut << "addiu $t3, $0, 0" << std::endl;
            assemblyOut << "j " << endLabel << std::endl;
            assemblyOut << "nop" << std::endl;

            assemblyOut << trueLabel << ":" << std::endl;
            assemblyOut << "addiu $t3, $0, 1" << std::endl;

            assemblyOut << endLabel << ":" << std::endl;
            break;
        }
        case Type::LOGIC_AND:
        {
            std::string trueLabel = generateUniqueLabel("trueLabel");
            std::string falseLabel = generateUniqueLabel("falseLabel");
            std::string endLabel = generateUniqueLabel("end");

            assemblyOut << "beq $t6, $0, " << falseLabel << std::endl;
            assemblyOut << "nop" << std::endl;

            assemblyOut << "beq $t1, $0, " << falseLabel << std::endl;
            assemblyOut << "nop" << std::endl;

            // both are true
            assemblyOut << "j " << trueLabel << std::endl;
            assemblyOut << "nop" << std::endl;

            assemblyOut << falseLabel << ":" << std::endl;
            assemblyOut << "addiu $t3, $0, 0" << std::endl;
            assemblyOut << "j " << endLabel << std::endl;
            assemblyOut << "nop" << std::endl;

            assemblyOut << trueLabel << ":" << std::endl;
            assemblyOut << "addiu $t3, $0, 1" << std::endl;

            assemblyOut << endLabel << ":" << std::endl;
            break;
        }
        case Type::BIT_OR:
        {
            assemblyOut << "or $t3, $t6, $t1" << std::endl;
            break;
        }
        case Type::BIT_XOR:
        {
            assemblyOut << "xor $t3, $t6, $t1" << std::endl;
            break;
        }
        case Type::BIT_AND:
        {
            assemblyOut << "and $t3, $t6, $t1" << std::endl;
            break;
        }
        case Type::EQUAL_EQUAL:
        {
            std::string trueLabel = generateUniqueLabel("trueLabel");
            std::string endLabel = generateUniqueLabel("end");

            assemblyOut << "beq $t6, $t1, " << trueLabel << std::endl;
            assemblyOut << "nop" << std::endl;

            assemblyOut << "addiu $t3, $0, 0" << std::endl;
            assemblyOut << "j " << endLabel << std::endl;
            assemblyOut << "nop" << std::endl;

            assemblyOut << trueLabel << ":" << std::endl;
            assemblyOut << "addiu $t3, $0, 1" << std::endl;

            assemblyOut << endLabel << ":" << std::endl;
            break;
        }
        case Type::BANG_EQUAL:
        {
            std::string trueLabel = generateUniqueLabel("trueLabel");
            std::string endLabel = generateUniqueLabel("end");

            assemblyOut << "bne $t6, $t1, " << trueLabel << std::endl;
            assemblyOut << "nop" << std::endl;

            assemblyOut << "addiu $t3, $0, 0" << std::endl;
            assemblyOut << "j " << endLabel << std::endl;
            assemblyOut << "nop" << std::endl;

            assemblyOut << trueLabel << ":" << std::endl;
            assemblyOut << "addiu $t3, $0, 1" << std::endl;

            assemblyOut << endLabel << ":" << std::endl;
            break;
        }
        case Type::PLUS:
        {
            assemblyOut << "add $t3, $t6, $t1" << std::endl;
            break;
        }
        case Type::MINUS:
        {
            assemblyOut << "sub $t3, $t6, $t1" << std::endl;
            break;
        }
        default:
        {
            throw std::runtime_error("AST_BinOp: Not Implemented Yet.\n");
            break;
        }
    }

    // store result in memory
    int relativeMemAddress = frame->getFrameSize() - frame->getMemOcc() - 5*4;
    frame->lastResultMemAddress = relativeMemAddress;
    assemblyOut << "sw $t3, " << relativeMemAddress << "($sp)" << std::endl;
}

AST_BinOp::~AST_BinOp(){
    delete left;
    delete right;
}

AST_UnOp::AST_UnOp(AST_UnOp::Type _type, AST* _operand):
    type(_type),
    operand(_operand)
{}

void AST_UnOp::generateFrames(Frame* _frame){
    frame = _frame;
    operand->generateFrames(_frame);
}

void AST_UnOp::compile(std::ostream &assemblyOut) {
    throw std::runtime_error("AST_UnOp: Not Implemented Yet.\n");
}

AST_UnOp::~AST_UnOp(){
    delete operand;
}
