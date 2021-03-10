#include "structure.hpp"
#include "expression.hpp"

AST_Sequence::AST_Sequence(AST* _first, AST* _second) :
    first(_first),
    second(_second)
{}

void AST_Sequence::generateFrames(Frame* _frame){
    frame = _frame;
    copySpecialParamsTo(first);
    first->generateFrames(_frame);
    copySpecialParamsTo(second);
    second->generateFrames(_frame);
}

AST* AST_Sequence::deepCopy(){
    AST* new_first = first->deepCopy();
    AST* new_second = second->deepCopy();
    return new AST_Sequence(new_first, new_second);
}

void AST_Sequence::compile(std::ostream &assemblyOut) {
    first->compile(assemblyOut);
    second->compile(assemblyOut);
}

AST_Sequence::~AST_Sequence(){
    delete first;
    delete second;
}

AST_FunDeclaration::AST_FunDeclaration(AST* _type, std::string* _name, AST* _body, std::vector<std::pair<AST*,std::string>>* _params) :
    type(_type),
    name(*_name),
    body(_body),
    params(_params)

{
    parity = 0;
    if(_params != nullptr) parity = _params->size();
}

void AST_FunDeclaration::generateFrames(Frame* _frame){
    frame = _frame;
    copySpecialParamsTo(type);
    type->generateFrames(frame);
    // we don't need to generate a new frame here since the block statement that will be the body
    // will handle generating the new frame
    if (body != nullptr) {
        copySpecialParamsTo(body);
        body->generateFrames(_frame);
        body->frame->isFun = true;
        // declare parameters as variables in the frame
        if(params != nullptr)
            for(std::pair<AST*,std::string> param: *params){
                body->frame->addVariable(param.second, param.first->getSize());
            }
    } 
}

AST* AST_FunDeclaration::deepCopy(){
    AST* new_type = type->deepCopy();
    AST* new_body = nullptr;
    if(body != nullptr){
        new_body = body->deepCopy();
    }
    std::vector<std::pair<AST*,std::string>>* new_params = nullptr;
    if(params != nullptr){
        new_params = new std::vector<std::pair<AST*,std::string>>();
        for(std::pair<AST*, std::string> param: *params){
            new_params->push_back(param);
        }
    }
    return new AST_FunDeclaration(new_type, &name, new_body, new_params);
}

void AST_FunDeclaration::compile(std::ostream &assemblyOut) {
    assemblyOut << std::endl << "# start function declaration for "<< name << std::endl;
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
        assemblyOut << ".frame	$fp, " << body->frame->getStoreSize() << " , $31" << std::endl;
        assemblyOut << ".mask	0x40000000,-4" << std::endl;
        assemblyOut << ".fmask	0x00000000,0" << std::endl;
        assemblyOut << ".set	noreorder" << std::endl;
        assemblyOut << ".set	nomacro" << std::endl;

        // increase size of current frame by required ammount for storing previous state data
        // currently storing only $31, and $fp
        assemblyOut << "addiu $sp, $sp, -" << body->frame->getStoreSize() << std::endl;
        assemblyOut << "sw $31, 8($sp)" << std::endl;
        assemblyOut << "sw $fp, 12($sp)" << std::endl;
        assemblyOut << "move $fp, $sp" << std::endl;

        // move stack pointer down to allocate space for temporary variables in frame
        assemblyOut << "addiu $sp, $sp, -" << body->frame->getVarSize() << std::endl;

        // copy over arguments from call
        if(params != nullptr){
            for(int i = 0, arg_i = params->size() - 1; i < params->size(); i++, arg_i--){
                // comment
                assemblyOut << std::endl << "# start loading parameter " << params->at(i).second << " in " << name << std::endl;
                // load from register
                if(arg_i < 4){
                    // If I didn't do it like this at runtime the strings got randomly truncated
                    std::string reg = std::string("$a") + std::to_string(arg_i);
                    regToVar(assemblyOut, body->frame, reg, params->at(i).second);
                }
                // load from memory
                else{
                    assemblyOut << "lw $t0, " << 4 * arg_i + body->frame->getStoreSize() << "($fp)" << std::endl;
                    regToVar(assemblyOut, body->frame, "$t0", params->at(i).second);
                }
                assemblyOut << "# loading parameter " << params->at(i).second << " in " << name << std::endl << std::endl;
            }
        }

        // body
        body->compile(assemblyOut);

        // move fp back to start of frame and re-instate previous frame
        assemblyOut << "move $sp, $fp" << std::endl;
        assemblyOut << "lw $31, 8($sp)" << std::endl;
        assemblyOut << "lw $fp, 12($sp)" << std::endl;
        assemblyOut << "addiu $sp, $sp, " << body->frame->getStoreSize() << std::endl;
        
        // jump back to wherever function was called from (this is only in place in case of void functions)
        // normally return statement will handle jumping
        assemblyOut << "jr $31" << std::endl;
        assemblyOut << "nop" << std::endl;

        // function footer
        assemblyOut << ".set	macro" << std::endl;
        assemblyOut << ".set	reorder" << std::endl;
        assemblyOut << ".end    " << name << std::endl;
        assemblyOut << ".size	" << name << ", .-" << name << std::endl;
    }
    assemblyOut << "# end function declaration for " << name << std::endl << std::endl;
}

AST_FunDeclaration::~AST_FunDeclaration() {
    delete type;
    if (body != nullptr) {
        delete body;
    }
    if (params != nullptr){
        delete params;
    }
}

AST_VarDeclaration::AST_VarDeclaration(AST* _type, std::string* _name, AST* _expr) :
    type(_type),
    name(*_name),
    expr(_expr)
{}

void AST_VarDeclaration::generateFrames(Frame* _frame){
    frame = _frame;
    type->generateFrames(_frame);
    if(expr != nullptr){
        copySpecialParamsTo(expr);
        expr->generateFrames(_frame);
    }
    
    _frame->addVariable(name, type->getSize());
}

AST* AST_VarDeclaration::deepCopy(){
    AST* new_type = type->deepCopy();
    AST* new_expr = nullptr;
    if(expr != nullptr){
        new_expr = expr->deepCopy();
    }
    return new AST_VarDeclaration(new_type, &name, new_expr);
}

void AST_VarDeclaration::compile(std::ostream &assemblyOut) {
    if (expr != nullptr) {
        assemblyOut << std::endl << "# start var dec with definition " << name << std::endl;

        expr->compile(assemblyOut);

        // load top of stack into register t0
        assemblyOut << "lw $t0, 8($sp)" << std::endl;
        assemblyOut << "addiu $sp, $sp, 8" << std::endl;

        regToVar(assemblyOut, frame, "$t0", name);
        
        assemblyOut << "# end var dec with definition " << name << std::endl << std::endl;
    }
}

AST_VarDeclaration::~AST_VarDeclaration() {
    delete type;
    if(expr != nullptr)
        delete expr; 
}

AST_ArrayDeclaration::AST_ArrayDeclaration(AST* _type, std::string* _name, int _size) :
    type(_type),
    name(*_name),
    size(_size)
{}

void AST_ArrayDeclaration::generateFrames(Frame* _frame){
    frame = _frame;
    type->generateFrames(_frame);
    // pointer size is always 4 bytes in a 32 bit system
    int pointer_size = 4;
    // pointer_size % 8 is too add padding after pointer.
    // this isn't useful for int's but when we need double word sized types this will save us
    // a lot of headaches.
    // no need to type_size since addVariable does that for us
    _frame->addVariable(name, pointer_size + pointer_size % 8 + type->getSize() * size  + type->getSize() * size % 8);
}

AST* AST_ArrayDeclaration::deepCopy(){
    AST* new_type = type->deepCopy();
    return new AST_ArrayDeclaration(new_type, &name, size);
}

void AST_ArrayDeclaration::compile(std::ostream &assemblyOut) {
    // get pointer to start of allocated memory space
    // always a double word away from allocated memory space
    assemblyOut << std::endl << "# start array declaration " << name << std::endl; 
    assemblyOut << "addiu $t0, $fp, -" << frame->getVarAddress(name).second + 8 << std::endl;
    regToVar(assemblyOut, frame, "$t0", name);
    assemblyOut << "# end array declaration " << name << std::endl << std::endl;
}

AST_ArrayDeclaration::~AST_ArrayDeclaration(){
    delete type;
}
