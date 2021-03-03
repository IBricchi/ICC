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
    assemblyOut << std::endl << "# start var definition " << name << std::endl;
    expr->compile(assemblyOut);

    // load top of stack into register t0
    assemblyOut << "lw $t0, 8($sp)" << std::endl;
    assemblyOut << "addiu $sp, $sp, 8" << std::endl;

    // store register data into variable's memory address
    assemblyOut << "sw $t0, -" << frame->getMemoryAddress(name) << "($fp)" << std::endl;
    
    assemblyOut << "# end var definition " << name << std::endl << std::endl;
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
    assemblyOut << std::endl << "# start function call " << functionName << std::endl;
    for (AST* arg : args) {
        // ...
        throw std::runtime_error("AST_FunctionCall: Not Implemented For Arguments Yet.\n");
    }

    assemblyOut << "jal " << functionName << std::endl;
    assemblyOut << "nop" << std::endl;
    
    assemblyOut << "sw $v0, 0($sp)" << std::endl;
    assemblyOut << "addiu $sp, $sp, -8" << std::endl;

    assemblyOut << "# end function call " << functionName << std::endl << std::endl;
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
    std::string binLabel = generateUniqueLabel("binOp");
    assemblyOut << std::endl << "# start " << binLabel << std::endl; 
    
    // load result of left expression into register
    // use $t6 as lower $t registers might be used in other compile functions called on right
    left->compile(assemblyOut);
    assemblyOut << "lw $t6, 8($sp)" << std::endl;

    switch (type) {
        case Type::LOGIC_OR:
        {
            assemblyOut << "# " << binLabel << " is &&" << std::endl;
            std::string trueLabel = generateUniqueLabel("trueLabel");
            std::string falseLabel = generateUniqueLabel("falseLabel");
            std::string endLabel = generateUniqueLabel("end");

            // evaluate first expression first => short-circuit evaluation
            assemblyOut << "bne $t6, $0, " << trueLabel << std::endl;
            assemblyOut << "nop" << std::endl;
            
            // load result of right expression into register
            right->compile(assemblyOut);
            assemblyOut << "lw $t1, 8($sp)" << std::endl;

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
            assemblyOut << "# " << binLabel << " is ||" << std::endl;
            std::string trueLabel = generateUniqueLabel("trueLabel");
            std::string falseLabel = generateUniqueLabel("falseLabel");
            std::string endLabel = generateUniqueLabel("end");

            assemblyOut << "beq $t6, $0, " << falseLabel << std::endl;
            assemblyOut << "nop" << std::endl;
            
            // load result of right expression into register
            right->compile(assemblyOut);
            assemblyOut << "lw $t1, 8($sp)" << std::endl;

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
            // load result of right expression into register
            right->compile(assemblyOut);
            assemblyOut << "lw $t1, 8($sp)" << std::endl;

            assemblyOut << "# " << binLabel << " is |" << std::endl;
            assemblyOut << "or $t3, $t6, $t1" << std::endl;
            break;
        }
        case Type::BIT_XOR:
        {
            // load result of right expression into register
            right->compile(assemblyOut);
            assemblyOut << "lw $t1, 8($sp)" << std::endl;
            
            assemblyOut << "# " << binLabel << " is ^" << std::endl;
            assemblyOut << "xor $t3, $t6, $t1" << std::endl;
            break;
        }
        case Type::BIT_AND:
        {
            // load result of right expression into register
            right->compile(assemblyOut);
            assemblyOut << "lw $t1, 8($sp)" << std::endl;
            
            
            assemblyOut << "# " << binLabel << " is &" << std::endl;
            assemblyOut << "and $t3, $t6, $t1" << std::endl;
            break;
        }
        case Type::EQUAL_EQUAL:
        {
            // load result of right expression into register
            right->compile(assemblyOut);
            assemblyOut << "lw $t1, 8($sp)" << std::endl;
                        
            assemblyOut << "# " << binLabel << " is ==" << std::endl;
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
            // load result of right expression into register
            right->compile(assemblyOut);
            assemblyOut << "lw $t1, 8($sp)" << std::endl;
                        
            assemblyOut << "# " << binLabel << " is !=" << std::endl;
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
        case Type::LESS:
        {
            // load result of right expression into register
            right->compile(assemblyOut);
            assemblyOut << "lw $t1, 8($sp)" << std::endl;

            assemblyOut << "# " << binLabel << " is <" << std::endl;
            assemblyOut << "slt $t3, $t6, $t1" << std::endl;
            break;
        }
        case Type::LESS_EQUAL:
        {
            // load result of right expression into register
            right->compile(assemblyOut);
            assemblyOut << "lw $t1, 8($sp)" << std::endl;

            assemblyOut << "# " << binLabel << " is <=" << std::endl;
            // less_equal if not greater
            std::string trueLabel = generateUniqueLabel("trueLabel");
            std::string endLabel = generateUniqueLabel("end");
            
            assemblyOut << "slt $t3, $t1, $t6" << std::endl;
            assemblyOut << "beq $t3, $0, " << trueLabel << std::endl;
            assemblyOut << "nop" << std::endl;

            assemblyOut << "addiu $t3, $0, 0" << std::endl;
            assemblyOut << "j " << endLabel << std::endl;
            assemblyOut << "nop" << std::endl;

            assemblyOut << trueLabel << ":" << std::endl;
            assemblyOut << "addiu $t3, $0, 1" << std::endl;

            assemblyOut << endLabel << ":" << std::endl;
            break;
        }
        case Type::GREATER:
        {
            // load result of right expression into register
            right->compile(assemblyOut);
            assemblyOut << "lw $t1, 8($sp)" << std::endl;

            assemblyOut << "# " << binLabel << " is >" << std::endl;
            assemblyOut << "slt $t3, $t1, $t6" << std::endl;
            break;
        }
        case Type::GREATER_EQUAL:
        {
            // load result of right expression into register
            right->compile(assemblyOut);
            assemblyOut << "lw $t1, 8($sp)" << std::endl;

            assemblyOut << "# " << binLabel << " is >=" << std::endl;
            // greater_equal if not less
            std::string trueLabel = generateUniqueLabel("trueLabel");
            std::string endLabel = generateUniqueLabel("end");
            
            assemblyOut << "slt $t3, $t6, $t1" << std::endl;
            assemblyOut << "beq $t3, $0, " << trueLabel << std::endl;
            assemblyOut << "nop" << std::endl;

            assemblyOut << "addiu $t3, $0, 0" << std::endl;
            assemblyOut << "j " << endLabel << std::endl;
            assemblyOut << "nop" << std::endl;

            assemblyOut << trueLabel << ":" << std::endl;
            assemblyOut << "addiu $t3, $0, 1" << std::endl;

            assemblyOut << endLabel << ":" << std::endl;
            break;
        }
        case Type::SHIFT_L:
        {
            // load result of right expression into register
            right->compile(assemblyOut);
            assemblyOut << "lw $t1, 8($sp)" << std::endl;

            assemblyOut << "# " << binLabel << " is <<" << std::endl;
            assemblyOut << "sll $t3, $t6, $t1" << std::endl;
            break;
        }
        case Type::SHIFT_R:
        {
            // load result of right expression into register
            right->compile(assemblyOut);
            assemblyOut << "lw $t1, 8($sp)" << std::endl;

            assemblyOut << "# " << binLabel << " is >>" << std::endl;
            assemblyOut << "srl $t3, $t6, $t1" << std::endl;
            break;
        }
        case Type::PLUS:
        {
            // load result of right expression into register
            right->compile(assemblyOut);
            assemblyOut << "lw $t1, 8($sp)" << std::endl;

            assemblyOut << "# " << binLabel << " is +" << std::endl;
            assemblyOut << "add $t3, $t6, $t1" << std::endl;
            break;
        }
        case Type::MINUS:
        {
            // load result of right expression into register
            right->compile(assemblyOut);
            assemblyOut << "lw $t1, 8($sp)" << std::endl;

            assemblyOut << "# " << binLabel << " is -" << std::endl;
            assemblyOut << "sub $t3, $t6, $t1" << std::endl;
            break;
        }
        case Type::STAR:
        {
            // load result of right expression into register
            right->compile(assemblyOut);
            assemblyOut << "lw $t1, 8($sp)" << std::endl;

            assemblyOut << "# " << binLabel << " is *" << std::endl;
            assemblyOut << "mult $t6, $t1" << std::endl;

            // only care about 32 least significant bits
            assemblyOut << "mflo $t3" << std::endl;
            break;
        }
        case Type::SLASH_F:
        {
            // load result of right expression into register
            right->compile(assemblyOut);
            assemblyOut << "lw $t1, 8($sp)" << std::endl;

            assemblyOut << "# " << binLabel << " is /" << std::endl;
            assemblyOut << "div $t6, $t1" << std::endl;

            // only care about quotient for fixed point division (get remainder using 'mfhi')
            assemblyOut << "mflo $t3" << std::endl;
            break;
        }
        case Type::PERCENT:
        {
            // load result of right expression into register
            right->compile(assemblyOut);
            assemblyOut << "lw $t1, 8($sp)" << std::endl;

            assemblyOut << "# " << binLabel << " is %" << std::endl;
            assemblyOut << "div $t6, $t1" << std::endl;

            // only care about remainder
            assemblyOut << "mfhi $t3" << std::endl;
            break;
        }
        default:
        {
            throw std::runtime_error("AST_BinOp: Not Implemented Yet.\n");
            break;
        }
    }

    // store result in memory
    assemblyOut << "sw $t3, 16($sp)" << std::endl;
    assemblyOut << "addiu $sp, $sp, 8" << std::endl;

    assemblyOut << "# end " << binLabel << std::endl << std::endl; 
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
