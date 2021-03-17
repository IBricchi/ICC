#include "expression.hpp"

AST_Assign::AST_Assign(AST* _assignee, AST* _expr):
    assignee(_assignee),
    expr(_expr)
{}

void AST_Assign::generateFrames(Frame* _frame){
    frame = _frame;
    assignee->returnPtr = true;
    assignee->generateFrames(_frame);
    expr->generateFrames(_frame);
}

AST* AST_Assign::deepCopy(){
    AST* new_assigne = assignee->deepCopy();
    AST* new_expr = expr->deepCopy();
    return new AST_Assign(new_assigne, new_expr);
}

void AST_Assign::compile(std::ostream &assemblyOut){
    std::string varType = assignee->getType()->getTypeName();

    std::string name = generateUniqueLabel("assignment");
    assemblyOut << std::endl << "# start " << name << " for " << varType << std::endl;

    // compile expresison
    expr->compile(assemblyOut);

    // compile assignee location
    assignee->compile(assemblyOut);

    // load memory address to assign to
    assemblyOut << "lw $t1, 8($sp)" << std::endl;

    if (varType == "float") {
        // load result of expression
        assemblyOut << "l.s $f4, 16($sp)" << std::endl;
        
        // assign memory address
        assemblyOut << "s.s $f4, 0($t1)" << std::endl;
    } else if (varType == "double") {
        // load result of expression
        assemblyOut << "l.d $f4, 16($sp)" << std::endl;
        
        // assign memory address
        assemblyOut << "s.d $f4, 0($t1)" << std::endl;
    } else {
        // load result of expression
        assemblyOut << "lw $t0, 16($sp)" << std::endl;

        // assign memory address
        assemblyOut << "sw $t0, 0($t1)" << std::endl;
    }

    // pop memory address
    assemblyOut << "addiu $sp, $sp, 8" << std::endl;

    assemblyOut << "# end " << name << " for " << varType << std::endl << std::endl;
}

// void regToVar(std::ostream &assemblyOut, Frame *frame, const std::__cxx11::string &reg, const std::__cxx11::string &var)
// void regToVar(std::ostream &assemblyOut, <error-type> *frame, const std::__cxx11::string &reg, const std::__cxx11::string &var)

AST_Assign::~AST_Assign(){
    delete assignee;
    delete expr;
}

AST_FunctionCall::AST_FunctionCall(std::string* _functionName, std::vector<AST*>* _args):
    functionName(*_functionName),
    args(_args)
{
    parity = 0;
    if(_args != nullptr) parity = _args->size();
}

void AST_FunctionCall::generateFrames(Frame* _frame){
    frame = _frame;
    if(args != nullptr){
        for(AST* arg: *args){
            arg->generateFrames(_frame);
        }
    }
}

AST* AST_FunctionCall::deepCopy(){
    std::vector<AST*>* new_args = nullptr;
    if(args!=nullptr){
        new_args = new std::vector<AST*>();
        for(AST* arg: *args){
            AST* new_arg = arg->deepCopy();
            new_args->push_back(new_arg);
        }
    }
    return new AST_FunctionCall(&functionName, new_args);
}

void AST_FunctionCall::compile(std::ostream &assemblyOut) {
    assemblyOut << std::endl << "# start function call " << functionName << std::endl;
    if(args != nullptr){
        // if even arguments add 4 bits of padding so sp remians properly padded
        if(args->size()%2){
            assemblyOut << "addiu $sp, $sp, -4" << std::endl;
        }
        for (int i = 0, arg_i = args->size()-1; i < args->size(); i++, arg_i--) {
            // compile all arguments
            args->at(i)->compile(assemblyOut);
            // store first 4 arguments in registers
            if(arg_i < 4){
                assemblyOut << "lw $a" << arg_i << ", 8($sp)" << std::endl; 
            }
            // move sp back 4 to remove extra padding
            assemblyOut << "addiu $sp, $sp, 4" << std::endl;
        }
        // move sp back by 4 to point to first argument 
        assemblyOut << "addiu $sp, $sp, 4" << std::endl;
    }

    assemblyOut << "jal " << functionName << std::endl;
    assemblyOut << "nop" << std::endl;
    
    // remove arguments from stack
    if(args  != nullptr){
        assemblyOut << "addiu $sp, $sp, " << 4 * (args->size() - (args->size()%2==0)) << std::endl;
    }

    std::string typeName = getTypeName();
    if(typeName == "float")
        assemblyOut << "s.s $f0, 0($sp)" << std::endl;
    else
        assemblyOut << "sw $v0, 0($sp)" << std::endl;
    assemblyOut << "addiu $sp, $sp, -8" << std::endl;

    assemblyOut << "# end function call " << functionName << std::endl << std::endl;
}

AST* AST_FunctionCall::getType(){
    return frame->getFnType(functionName);
}

int AST_FunctionCall::getBytes(){
    return getType()->getBytes();
}

std::string AST_FunctionCall::getTypeName(){
    return frame->getFnType(functionName)->getTypeName();
}

AST_FunctionCall::~AST_FunctionCall() {
    if(args != nullptr){
        for (AST* arg : *args) {
            if (arg != nullptr) {
                delete arg;
            }
        }
        delete args;
    }
}

AST_BinOp::AST_BinOp(AST_BinOp::Type _type, AST* _left, AST* _right):
    type(_type),
    left(_left),
    right(_right)
{}

void AST_BinOp::generateFrames(Frame* _frame){
    frame = _frame;

    // if type is array, remove left-assignment param from index expression
    if(type == Type::ARRAY){
        left->returnPtr = true;
    }
    left->generateFrames(_frame);
    right->generateFrames(_frame);
}

AST* AST_BinOp::deepCopy(){
    AST* new_left = left->deepCopy();
    AST* new_right = right->deepCopy();
    return new AST_BinOp(type, new_left, new_right);
}

void AST_BinOp::compile(std::ostream &assemblyOut) {
    this->getType(); // ensure that interalDataType is initialised
    std::string varType = this->internalDataType->getTypeName();

    std::string binLabel = generateUniqueLabel("binOp");
    assemblyOut << std::endl << "# start " << binLabel << std::endl; 
    
    // compile left expression
    left->compile(assemblyOut);

    if (varType == "float") {
        // storing result in memory is done in every case statement because results are
        // int (boolean) or float
        switch (type) {
            case Type::EQUAL_EQUAL:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "l.s $f4, 16($sp)" << std::endl;
                assemblyOut << "l.s $f5, 8($sp)" << std::endl;
                            
                assemblyOut << "# " << binLabel << " is float ==" << std::endl;
                std::string trueLabel = generateUniqueLabel("trueLabel");
                std::string endLabel = generateUniqueLabel("end");

                assemblyOut << "c.eq.s $f4, $f5" << std::endl;
                assemblyOut << "bc1t " << trueLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << "addiu $t2, $0, 0" << std::endl;
                assemblyOut << "j " << endLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << trueLabel << ":" << std::endl;
                assemblyOut << "addiu $t2, $0, 1" << std::endl;

                assemblyOut << endLabel << ":" << std::endl;

                // store result in memory
                assemblyOut << "sw $t2, 16($sp)" << std::endl;
                break;
            }
            case Type::BANG_EQUAL:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "l.s $f4, 16($sp)" << std::endl;
                assemblyOut << "l.s $f5, 8($sp)" << std::endl;
                            
                assemblyOut << "# " << binLabel << " is float !=" << std::endl;
                std::string trueLabel = generateUniqueLabel("trueLabel");
                std::string endLabel = generateUniqueLabel("end");

                assemblyOut << "c.eq.s $f4, $f5" << std::endl;
                assemblyOut << "bc1f " << trueLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << "addiu $t2, $0, 0" << std::endl;
                assemblyOut << "j " << endLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << trueLabel << ":" << std::endl;
                assemblyOut << "addiu $t2, $0, 1" << std::endl;

                assemblyOut << endLabel << ":" << std::endl;

                // store result in memory
                assemblyOut << "sw $t2, 16($sp)" << std::endl;
                break;
            }
            case Type::LESS:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "l.s $f4, 16($sp)" << std::endl;
                assemblyOut << "l.s $f5, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is float <" << std::endl;
                std::string trueLabel = generateUniqueLabel("trueLabel");
                std::string endLabel = generateUniqueLabel("end");

                assemblyOut << "c.lt.s $f4, $f5" << std::endl;
                assemblyOut << "bc1t " << trueLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << "li $t2, 0" << std::endl;
                assemblyOut << "j " << endLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << trueLabel << ":" << std::endl;
                assemblyOut << "li $t2, 1" << std::endl;

                assemblyOut << endLabel << ":" << std::endl;

                // store result in memory
                assemblyOut << "sw $t2, 16($sp)" << std::endl;
                break;
            }
            case Type::LESS_EQUAL:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "l.s $f4, 16($sp)" << std::endl;
                assemblyOut << "l.s $f5, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is float <=" << std::endl;
                std::string trueLabel = generateUniqueLabel("trueLabel");
                std::string endLabel = generateUniqueLabel("end");

                assemblyOut << "c.le.s $f4, $f5" << std::endl;
                assemblyOut << "bc1t " << trueLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << "li $t2, 0" << std::endl;
                assemblyOut << "j " << endLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << trueLabel << ":" << std::endl;
                assemblyOut << "li $t2, 1" << std::endl;

                assemblyOut << endLabel << ":" << std::endl;

                // store result in memory
                assemblyOut << "sw $t2, 16($sp)" << std::endl;
                break;
            }
            case Type::GREATER:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "l.s $f4, 16($sp)" << std::endl;
                assemblyOut << "l.s $f5, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is float >" << std::endl;
                std::string trueLabel = generateUniqueLabel("trueLabel");
                std::string endLabel = generateUniqueLabel("end");

                assemblyOut << "c.lt.s $f5, $f4" << std::endl;
                assemblyOut << "bc1t " << trueLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << "li $t2, 0" << std::endl;
                assemblyOut << "j " << endLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << trueLabel << ":" << std::endl;
                assemblyOut << "li $t2, 1" << std::endl;

                assemblyOut << endLabel << ":" << std::endl;

                // store result in memory
                assemblyOut << "sw $t2, 16($sp)" << std::endl;
                break;
            }
            case Type::GREATER_EQUAL:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "l.s $f4, 16($sp)" << std::endl;
                assemblyOut << "l.s $f5, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is float >=" << std::endl;
                std::string trueLabel = generateUniqueLabel("trueLabel");
                std::string endLabel = generateUniqueLabel("end");

                assemblyOut << "c.le.s $f5, $f4" << std::endl;
                assemblyOut << "bc1t " << trueLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << "li $t2, 0" << std::endl;
                assemblyOut << "j " << endLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << trueLabel << ":" << std::endl;
                assemblyOut << "li $t2, 1" << std::endl;

                assemblyOut << endLabel << ":" << std::endl;

                // store result in memory
                assemblyOut << "sw $t2, 16($sp)" << std::endl;
                break;
            }
            case Type::PLUS:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "l.s $f4, 16($sp)" << std::endl;
                assemblyOut << "l.s $f5, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is float +" << std::endl;
                assemblyOut << "add.s $f6, $f4, $f5" << std::endl;

                // store result in memory
                assemblyOut << "s.s $f6, 16($sp)" << std::endl;
                break;
            }
            case Type::MINUS:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "l.s $f4, 16($sp)" << std::endl;
                assemblyOut << "l.s $f5, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is float -" << std::endl;
                assemblyOut << "sub.s $f6, $f4, $f5" << std::endl;

                // store result in memory
                assemblyOut << "s.s $f6, 16($sp)" << std::endl;
                break;
            }
            case Type::STAR:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "l.s $f4, 16($sp)" << std::endl;
                assemblyOut << "l.s $f5, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is float *" << std::endl;
                assemblyOut << "mul.s $f6, $f4, $f5" << std::endl;

                // store result in memory
                assemblyOut << "s.s $f6, 16($sp)" << std::endl;
                break;
            }
            case Type::SLASH_F:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "l.s $f4, 16($sp)" << std::endl;
                assemblyOut << "l.s $f5, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is float /" << std::endl;
                assemblyOut << "div.s $f6, $f4, $f5" << std::endl;

                // store result in memory
                assemblyOut << "s.s $f6, 16($sp)" << std::endl;
                break;
            }
            default:
            {
                throw std::runtime_error("AST_BinOp: Float Not Implemented Yet.\n");
                break;
            }
        }
    } else if (varType == "double") {
        // storing result in memory is done in every case statement because results are
        // int (boolean) or double
        switch (type) {
            case Type::EQUAL_EQUAL:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "l.d $f4, 16($sp)" << std::endl;
                assemblyOut << "l.d $f6, 8($sp)" << std::endl;
                            
                assemblyOut << "# " << binLabel << " is double ==" << std::endl;
                std::string trueLabel = generateUniqueLabel("trueLabel");
                std::string endLabel = generateUniqueLabel("end");

                assemblyOut << "c.eq.d $f4, $f6" << std::endl;
                assemblyOut << "bc1t " << trueLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << "addiu $t2, $0, 0" << std::endl;
                assemblyOut << "j " << endLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << trueLabel << ":" << std::endl;
                assemblyOut << "addiu $t2, $0, 1" << std::endl;

                assemblyOut << endLabel << ":" << std::endl;

                // store result in memory
                assemblyOut << "sw $t2, 16($sp)" << std::endl;
                break;
            }
            case Type::BANG_EQUAL:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "l.d $f4, 16($sp)" << std::endl;
                assemblyOut << "l.d $f6, 8($sp)" << std::endl;
                            
                assemblyOut << "# " << binLabel << " is double !=" << std::endl;
                std::string trueLabel = generateUniqueLabel("trueLabel");
                std::string endLabel = generateUniqueLabel("end");

                assemblyOut << "c.eq.d $f4, $f6" << std::endl;
                assemblyOut << "bc1f " << trueLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << "addiu $t2, $0, 0" << std::endl;
                assemblyOut << "j " << endLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << trueLabel << ":" << std::endl;
                assemblyOut << "addiu $t2, $0, 1" << std::endl;

                assemblyOut << endLabel << ":" << std::endl;

                // store result in memory
                assemblyOut << "sw $t2, 16($sp)" << std::endl;
                break;
            }
            case Type::LESS:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "l.d $f4, 16($sp)" << std::endl;
                assemblyOut << "l.d $f6, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is double <" << std::endl;
                std::string trueLabel = generateUniqueLabel("trueLabel");
                std::string endLabel = generateUniqueLabel("end");

                assemblyOut << "c.lt.d $f4, $f6" << std::endl;
                assemblyOut << "bc1t " << trueLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << "li $t2, 0" << std::endl;
                assemblyOut << "j " << endLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << trueLabel << ":" << std::endl;
                assemblyOut << "li $t2, 1" << std::endl;

                assemblyOut << endLabel << ":" << std::endl;

                // store result in memory
                assemblyOut << "sw $t2, 16($sp)" << std::endl;
                break;
            }
            case Type::LESS_EQUAL:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "l.d $f4, 16($sp)" << std::endl;
                assemblyOut << "l.d $f6, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is double <=" << std::endl;
                std::string trueLabel = generateUniqueLabel("trueLabel");
                std::string endLabel = generateUniqueLabel("end");

                assemblyOut << "c.le.d $f4, $f6" << std::endl;
                assemblyOut << "bc1t " << trueLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << "li $t2, 0" << std::endl;
                assemblyOut << "j " << endLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << trueLabel << ":" << std::endl;
                assemblyOut << "li $t2, 1" << std::endl;

                assemblyOut << endLabel << ":" << std::endl;

                // store result in memory
                assemblyOut << "sw $t2, 16($sp)" << std::endl;
                break;
            }
            case Type::GREATER:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "l.d $f4, 16($sp)" << std::endl;
                assemblyOut << "l.d $f6, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is double >" << std::endl;
                std::string trueLabel = generateUniqueLabel("trueLabel");
                std::string endLabel = generateUniqueLabel("end");

                assemblyOut << "c.lt.d $f6, $f4" << std::endl;
                assemblyOut << "bc1t " << trueLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << "li $t2, 0" << std::endl;
                assemblyOut << "j " << endLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << trueLabel << ":" << std::endl;
                assemblyOut << "li $t2, 1" << std::endl;

                assemblyOut << endLabel << ":" << std::endl;

                // store result in memory
                assemblyOut << "sw $t2, 16($sp)" << std::endl;
                break;
            }
            case Type::GREATER_EQUAL:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "l.d $f4, 16($sp)" << std::endl;
                assemblyOut << "l.d $f6, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is double >=" << std::endl;
                std::string trueLabel = generateUniqueLabel("trueLabel");
                std::string endLabel = generateUniqueLabel("end");

                assemblyOut << "c.le.d $f6, $f4" << std::endl;
                assemblyOut << "bc1t " << trueLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << "li $t2, 0" << std::endl;
                assemblyOut << "j " << endLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << trueLabel << ":" << std::endl;
                assemblyOut << "li $t2, 1" << std::endl;

                assemblyOut << endLabel << ":" << std::endl;

                // store result in memory
                assemblyOut << "sw $t2, 16($sp)" << std::endl;
                break;
            }
            case Type::PLUS:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "l.d $f4, 16($sp)" << std::endl;
                assemblyOut << "l.d $f6, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is double +" << std::endl;
                assemblyOut << "add.d $f8, $f4, $f6" << std::endl;

                // store result in memory
                assemblyOut << "s.d $f8, 16($sp)" << std::endl;
                break;
            }
            case Type::MINUS:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "l.d $f4, 16($sp)" << std::endl;
                assemblyOut << "l.d $f6, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is double -" << std::endl;
                assemblyOut << "sub.d $f8, $f4, $f6" << std::endl;

                // store result in memory
                assemblyOut << "s.d $f8, 16($sp)" << std::endl;
                break;
            }
            case Type::STAR:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "l.d $f4, 16($sp)" << std::endl;
                assemblyOut << "l.d $f6, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is double *" << std::endl;
                assemblyOut << "mul.d $f8, $f4, $f6" << std::endl;

                // store result in memory
                assemblyOut << "s.d $f8, 16($sp)" << std::endl;
                break;
            }
            case Type::SLASH_F:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "l.d $f4, 16($sp)" << std::endl;
                assemblyOut << "l.d $f6, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is double /" << std::endl;
                assemblyOut << "div.d $f8, $f4, $f6" << std::endl;

                // store result in memory
                assemblyOut << "s.d $f8, 16($sp)" << std::endl;
                break;
            }
            default:
            {
                throw std::runtime_error("AST_BinOp: Double Not Implemented Yet.\n");
                break;
            }
        }
    } else {
        switch (type) {
            case Type::LOGIC_OR:
            {
                assemblyOut << "# " << binLabel << " is &&" << std::endl;
                std::string trueLabel = generateUniqueLabel("trueLabel");
                std::string falseLabel = generateUniqueLabel("falseLabel");
                std::string endLabel = generateUniqueLabel("end");

                // evaluate first expression first => short-circuit evaluation
                assemblyOut << "lw $t0, 8($sp)" << std::endl;
                assemblyOut << "bne $t0, $0, " << trueLabel << std::endl;
                assemblyOut << "nop" << std::endl;
                
                // load result of right expression into register
                right->compile(assemblyOut);
                assemblyOut << "lw $t1, 8($sp)" << std::endl;

                assemblyOut << "bne $t1, $0, " << trueLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << falseLabel << ":" << std::endl;
                assemblyOut << "addiu $t2, $0, 0" << std::endl;
                assemblyOut << "j " << endLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << trueLabel << ":" << std::endl;
                assemblyOut << "addiu $t2, $0, 1" << std::endl;

                assemblyOut << endLabel << ":" << std::endl;
                break;
            }
            case Type::LOGIC_AND:
            {
                assemblyOut << "# " << binLabel << " is &&" << std::endl;
                std::string trueLabel = generateUniqueLabel("trueLabel");
                std::string falseLabel = generateUniqueLabel("falseLabel");
                std::string endLabel = generateUniqueLabel("end");

                // evaluate first expression first => short-circuit evaluation
                assemblyOut << "lw $t0, 8($sp)" << std::endl;
                assemblyOut << "beq $t0, $0, " << falseLabel << std::endl;
                assemblyOut << "nop" << std::endl;
                
                // load result of right expression into register
                right->compile(assemblyOut);
                assemblyOut << "lw $t1, 8($sp)" << std::endl;
                
                assemblyOut << "beq $t1, $0, " << falseLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << trueLabel << ":" << std::endl;
                assemblyOut << "addiu $t2, $0, 1" << std::endl;
                assemblyOut << "j " << endLabel << std::endl;
                assemblyOut << "nop" << std::endl;
                
                assemblyOut << falseLabel << ":" << std::endl;
                assemblyOut << "addiu $t2, $0, 0" << std::endl;
            
                assemblyOut << endLabel << ":" << std::endl;
                break;
            }
            case Type::BIT_OR:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "lw $t0, 16($sp)" << std::endl;
                assemblyOut << "lw $t1, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is |" << std::endl;
                assemblyOut << "or $t2, $t0, $t1" << std::endl;
                break;
            }
            case Type::BIT_XOR:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "lw $t0, 16($sp)" << std::endl;
                assemblyOut << "lw $t1, 8($sp)" << std::endl;
                
                assemblyOut << "# " << binLabel << " is ^" << std::endl;
                assemblyOut << "xor $t2, $t0, $t1" << std::endl;
                break;
            }
            case Type::BIT_AND:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "lw $t0, 16($sp)" << std::endl;
                assemblyOut << "lw $t1, 8($sp)" << std::endl;
                
                
                assemblyOut << "# " << binLabel << " is &" << std::endl;
                assemblyOut << "and $t2, $t0, $t1" << std::endl;
                break;
            }
            case Type::EQUAL_EQUAL:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "lw $t0, 16($sp)" << std::endl;
                assemblyOut << "lw $t1, 8($sp)" << std::endl;
                            
                assemblyOut << "# " << binLabel << " is ==" << std::endl;
                std::string trueLabel = generateUniqueLabel("trueLabel");
                std::string endLabel = generateUniqueLabel("end");

                assemblyOut << "beq $t0, $t1, " << trueLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << "addiu $t2, $0, 0" << std::endl;
                assemblyOut << "j " << endLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << trueLabel << ":" << std::endl;
                assemblyOut << "addiu $t2, $0, 1" << std::endl;

                assemblyOut << endLabel << ":" << std::endl;
                break;
            }
            case Type::BANG_EQUAL:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "lw $t0, 16($sp)" << std::endl;
                assemblyOut << "lw $t1, 8($sp)" << std::endl;
                            
                assemblyOut << "# " << binLabel << " is !=" << std::endl;
                std::string trueLabel = generateUniqueLabel("trueLabel");
                std::string endLabel = generateUniqueLabel("end");

                assemblyOut << "bne $t0, $t1, " << trueLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << "addiu $t2, $0, 0" << std::endl;
                assemblyOut << "j " << endLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << trueLabel << ":" << std::endl;
                assemblyOut << "addiu $t2, $0, 1" << std::endl;

                assemblyOut << endLabel << ":" << std::endl;
                break;
            }
            case Type::LESS:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "lw $t0, 16($sp)" << std::endl;
                assemblyOut << "lw $t1, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is <" << std::endl;
                assemblyOut << "slt $t2, $t0, $t1" << std::endl;
                break;
            }
            case Type::LESS_EQUAL:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "lw $t0, 16($sp)" << std::endl;
                assemblyOut << "lw $t1, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is <=" << std::endl;
                // less_equal if not greater
                std::string trueLabel = generateUniqueLabel("trueLabel");
                std::string endLabel = generateUniqueLabel("end");
                
                assemblyOut << "slt $t2, $t1, $t0" << std::endl;
                assemblyOut << "beq $t2, $0, " << trueLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << "addiu $t2, $0, 0" << std::endl;
                assemblyOut << "j " << endLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << trueLabel << ":" << std::endl;
                assemblyOut << "addiu $t2, $0, 1" << std::endl;

                assemblyOut << endLabel << ":" << std::endl;
                break;
            }
            case Type::GREATER:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "lw $t0, 16($sp)" << std::endl;
                assemblyOut << "lw $t1, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is >" << std::endl;
                assemblyOut << "slt $t2, $t1, $t0" << std::endl;
                break;
            }
            case Type::GREATER_EQUAL:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "lw $t0, 16($sp)" << std::endl;
                assemblyOut << "lw $t1, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is >=" << std::endl;
                // greater_equal if not less
                std::string trueLabel = generateUniqueLabel("trueLabel");
                std::string endLabel = generateUniqueLabel("end");
                
                assemblyOut << "slt $t2, $t0, $t1" << std::endl;
                assemblyOut << "beq $t2, $0, " << trueLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << "addiu $t2, $0, 0" << std::endl;
                assemblyOut << "j " << endLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                assemblyOut << trueLabel << ":" << std::endl;
                assemblyOut << "addiu $t2, $0, 1" << std::endl;

                assemblyOut << endLabel << ":" << std::endl;
                break;
            }
            case Type::SHIFT_L:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "lw $t0, 16($sp)" << std::endl;
                assemblyOut << "lw $t1, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is <<" << std::endl;
                assemblyOut << "sll $t2, $t0, $t1" << std::endl;
                break;
            }
            case Type::SHIFT_R:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "lw $t0, 16($sp)" << std::endl;
                assemblyOut << "lw $t1, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is >>" << std::endl;
                assemblyOut << "srl $t2, $t0, $t1" << std::endl;
                break;
            }
            case Type::PLUS:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "lw $t0, 16($sp)" << std::endl;
                assemblyOut << "lw $t1, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is +" << std::endl;
                assemblyOut << "add $t2, $t0, $t1" << std::endl;
                break;
            }
            case Type::MINUS:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "lw $t0, 16($sp)" << std::endl;
                assemblyOut << "lw $t1, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is -" << std::endl;
                assemblyOut << "sub $t2, $t0, $t1" << std::endl;
                break;
            }
            case Type::STAR:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "lw $t0, 16($sp)" << std::endl;
                assemblyOut << "lw $t1, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is *" << std::endl;
                assemblyOut << "mult $t0, $t1" << std::endl;

                // only care about 32 least significant bits
                assemblyOut << "mflo $t2" << std::endl;
                break;
            }
            case Type::SLASH_F:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "lw $t0, 16($sp)" << std::endl;
                assemblyOut << "lw $t1, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is /" << std::endl;
                assemblyOut << "div $t0, $t1" << std::endl;

                // only care about quotient for fixed point division (get remainder using 'mfhi')
                assemblyOut << "mflo $t2" << std::endl;
                break;
            }
            case Type::PERCENT:
            {
                // load result of right expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "lw $t0, 16($sp)" << std::endl;
                assemblyOut << "lw $t1, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " is %" << std::endl;
                assemblyOut << "div $t0, $t1" << std::endl;

                // only care about remainder
                assemblyOut << "mfhi $t2" << std::endl;
                break;
            }
            case Type::ARRAY:
            {
                // load result of index expression into register
                right->compile(assemblyOut);
                
                assemblyOut << "lw $t0, 16($sp)" << std::endl;
                // if left of assign de-reference left variable
                // if(specialParams[(int)SpecialParam::KEEP_AS_POINTER])
                //     assemblyOut << "lw $t0, 0($t0)" << std::endl;
                assemblyOut << "lw $t1, 8($sp)" << std::endl;

                assemblyOut << "# " << binLabel << " [] " << std::endl;
                assemblyOut << "addiu $t2, $0, " << getBytes() << std::endl;
                assemblyOut << "multu $t1, $t2" << std::endl;
                assemblyOut << "mflo $t1" << std::endl;
                assemblyOut << "sub $t2, $t0, $t1" << std::endl;
                // if not left of assign load value
                if(!returnPtr)
                    assemblyOut << "lw $t2, 0($t2)" << std::endl;
                
                break;
            }
            default:
            {
                throw std::runtime_error("AST_BinOp: Not Implemented Yet.\n");
                break;
            }
        }

        // store result in memory
        assemblyOut << "sw $t2, 16($sp)" << std::endl;
    }
    
    assemblyOut << "addiu $sp, $sp, 8" << std::endl;

    assemblyOut << "# end " << binLabel << std::endl << std::endl; 
}

void AST_BinOp::setType(std::string newType) { 
    this->dataType = new AST_Type(&newType);
}

AST* AST_BinOp::getType(){
    if (this->internalDataType == nullptr) {
        // save internal type
        AST* left_type = left->getType();
        if(type == Type::ARRAY){
            left_type = left_type->getType();
        }
        this->internalDataType = left_type;

        // change type to int (result is boolean)
        if (type == Type::EQUAL_EQUAL || type == Type::BANG_EQUAL || type == Type::LESS
            || type == Type::LESS_EQUAL || type == Type::GREATER || type == Type::GREATER_EQUAL) {
            this->setType("int");
        }
    }

    if (this->dataType == nullptr) {
        // assuming left and right have same type
        // we don't need to implement implicit casting so this should be fine
        AST* left_type = left->getType();
        if(type == Type::ARRAY){
            left_type = left_type->getType();
        }
        this->dataType = left_type;
    }
    return this->dataType;
}

int AST_BinOp::getBytes(){
    // assuming left and right have same type
    // we don't need to implement implicit casting so this should be fine
    int bytes = left->getBytes();
    if(type == Type::ARRAY){
        bytes = left->getType()->getType()->getBytes();
    }
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

AST* AST_UnOp::deepCopy(){
    AST* new_operand = operand->deepCopy();
    return new AST_UnOp(type, new_operand);
}

void AST_UnOp::compile(std::ostream &assemblyOut) {
    std::string varType = this->getType()->getTypeName();

    std::string unLabel = generateUniqueLabel("unOp");
    assemblyOut << std::endl << "# start " << unLabel << std::endl;

    operand->compile(assemblyOut);

    if (varType == "float") {
        assemblyOut << "l.s $f4, 8($sp)" << std::endl;

        switch (type) {
            case Type::MINUS:
            {
                assemblyOut << "# " << unLabel << " is float -" << std::endl;

                assemblyOut << "neg.s $f6, $f4" << std::endl;
                break;
            }
            default:
            {
                throw std::runtime_error("AST_BinOp: Float Not Implemented Yet.\n");
                break;
            }
        }

        // push onto operand stack
        assemblyOut << "s.s $f6, 8($sp)" << std::endl;
    } else if (varType == "double") {
        assemblyOut << "l.d $f4, 8($sp)" << std::endl;

        switch (type) {
            case Type::MINUS:
            {
                assemblyOut << "# " << unLabel << " is double -" << std::endl;

                assemblyOut << "neg.d $f6, $f4" << std::endl;
                break;
            }
            default:
            {
                throw std::runtime_error("AST_BinOp: Double Not Implemented Yet.\n");
                break;
            }
        }

        // push onto operand stack
        assemblyOut << "s.d $f6, 8($sp)" << std::endl;
    } else {
        assemblyOut << "lw $t0, 8($sp)" << std::endl;

        switch (type) {
            case Type::BANG:
            {
                // if 0, set to 1 else, set to 0
                assemblyOut << "# " << unLabel << " is !" << std::endl;

                std::string currentlyFalseLabel = generateUniqueLabel("currentlyFalseLabel");
                std::string endLabel = generateUniqueLabel("end");

                assemblyOut << "beq $t0, $0, " << currentlyFalseLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                // currently true
                assemblyOut << "addiu $t1, $0, 0" << std::endl;
                assemblyOut << "j " << endLabel << std::endl;
                assemblyOut << "nop" << std::endl;

                // currently false
                assemblyOut << currentlyFalseLabel <<  ":" << std::endl;
                assemblyOut << "addiu $t1, $0, 1" << std::endl;
                
                assemblyOut << endLabel << ":" << std::endl;
                break;
            }
            case Type::NOT:
            {
                assemblyOut << "# " << unLabel << " is ~" << std::endl;

                assemblyOut << "nor $t1, $t0, $t0" << std::endl;
                break;
            }
            case Type::MINUS:
            {
                assemblyOut << "# " << unLabel << " is -" << std::endl;

                assemblyOut << "subu $t1, $0, $t0" << std::endl;
                break;
            }
            case Type::PRE_INCREMENT:
            {
                assemblyOut << "# " << unLabel << " is pre ++" << std::endl;

                assemblyOut << "addiu $t1, $t0, 1" << std::endl;

                // update variable
                operand->updateVariable(assemblyOut, frame, "$t1");
                break;
            }
            case Type::PRE_DECREMENT:
            {
                assemblyOut << "# " << unLabel << " is pre --" << std::endl;

                assemblyOut << "addiu $t1, $t0, -1" << std::endl;

                // update variable
                operand->updateVariable(assemblyOut, frame, "$t1");
                break;
            }
            case Type::POST_INCREMENT:
            {
                assemblyOut << "# " << unLabel << " is post ++" << std::endl;

                // push onto operand stack
                assemblyOut << "sw $t0, 8($sp)" << std::endl;

                assemblyOut << "addiu $t1, $t0, 1" << std::endl;

                // update variable
                operand->updateVariable(assemblyOut, frame, "$t1");
                break;
            }
            case Type::POST_DECREMENT:
            {
                assemblyOut << "# " << unLabel << " is post --" << std::endl;
                
                // push onto operand stack
                assemblyOut << "sw $t0, 8($sp)" << std::endl;

                assemblyOut << "addiu $t1, $t0, -1" << std::endl;

                // update variable
                operand->updateVariable(assemblyOut, frame, "$t1");
                break;
            }
            default:
            {
                throw std::runtime_error("AST_BinOp: Not Implemented Yet.\n");
                break;
            }
        }

        // push onto operand stack
        if (type != Type::POST_DECREMENT && type != Type::POST_INCREMENT) {
            assemblyOut << "sw $t1, 8($sp)" << std::endl;
        }
    }
    
    assemblyOut << "# end " << unLabel << std::endl << std::endl; 
}

AST* AST_UnOp::getType(){
    return this->operand->getType();
}

int AST_UnOp::getBytes(){
    return this->operand->getBytes();
}

AST_UnOp::~AST_UnOp(){
    delete operand;
}
