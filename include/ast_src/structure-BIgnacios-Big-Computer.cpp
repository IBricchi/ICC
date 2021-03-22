#include "structure.hpp"
#include "expression.hpp"

AST_Sequence::AST_Sequence(AST* _first, AST* _second) :
    first(_first),
    second(_second)
{}

void AST_Sequence::generateFrames(Frame* _frame){
    frame = _frame;
    first->generateFrames(_frame);
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

void AST_Sequence::setStructName(std::string newName) {
    this->structName = newName;
}

std::string AST_Sequence::getStructName() {
    return this->structName;
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
    type->generateFrames(frame);
    frame->addFunction(name, this);
    // we don't need to generate a new frame here since the block statement that will be the body
    // will handle generating the new frame
    if (body != nullptr) {
        body->generateFrames(_frame);
        body->frame->fn = this;
        // declare parameters as variables in the frame
        if(params != nullptr)
            for(std::pair<AST*,std::string> param: *params){
                body->frame->addVariable(param.second, param.first, param.first->getBytes());
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
        assemblyOut << "addiu $sp, $sp, -" << body->frame->getVarStoreSize() << std::endl;

        // copy over arguments from call
        if(params != nullptr){
            // state variables
            bool allowFReg = true;
            bool loadFromReg = true;
            int availableAReg = 0;
            int availableFReg = 12;
            int memOffset = 0;
            // i is position in vector, arg_i is position in argument order
            for(int i = params->size() - 1, arg_i = 0; arg_i < params->size(); i--, arg_i++){
                // parameterInfo
                std::pair<AST*, std::string> param = params->at(i);
                std::string paramTypeName = param.first->getTypeName();

                // comment
                assemblyOut << std::endl << "# start loading parameter " << param.second << " in " << name << std::endl;
                            
                bool useMem = !loadFromReg;

                // load from register
                if(loadFromReg){
                    if(paramTypeName == "float" || paramTypeName == "double"){
                        // this part is the same for floats and doubles
                        if(allowFReg){
                            assemblyOut << "# (reading a " << paramTypeName << " type from f reg)" << std::endl;
                            std::string reg = std::string("$f") + std::to_string(availableFReg);
                            regToVar(assemblyOut, body->frame, reg, param.second);
                            
                            // update state
                            availableFReg += 2;
                            availableAReg++;
                            memOffset += 4;
                            if(paramTypeName == "double"){
                                availableAReg++;
                                memOffset += 4;
                            }

                            if(availableFReg == 16)
                                allowFReg = false;
                            if(availableAReg == 4)
                                loadFromReg = false;
                        }
                        else{
                            assemblyOut << "# (reading a " << paramTypeName << " type from a reg)" << std::endl;
                            
                            if(paramTypeName == "double"){
                                if(availableAReg % 2){
                                    memOffset += 4;
                                    availableAReg++;
                                }
                                
                                if(availableAReg < 4){
                                    std::string reg = std::string("$a") + std::to_string(availableAReg);
                                    std::string reg_2 = std::string("$a") + std::to_string(availableAReg+1);
                                    regToVar(assemblyOut, body->frame, reg, param.second, reg_2);

                                    // update state
                                    availableAReg += 2;
                                    memOffset += 8;
                                }
                                else{
                                    loadFromReg = false;
                                    useMem = true;
                                }
                            }
                            else{
                                std::string reg = std::string("$a") + std::to_string(availableAReg);
                                regToVar(assemblyOut, body->frame, reg, param.second);
                                
                                // update state
                                availableAReg++;
                                memOffset += 4;
                            }

                            // check state
                            if(availableAReg == 4)
                                loadFromReg = false;
                        }
                    }
                    else{
                        assemblyOut << "# (reading a integer type)" << std::endl;
                        std::string reg = std::string("$a") + std::to_string(availableAReg);
                        regToVar(assemblyOut, body->frame, reg, param.second);

                        // update state
                        availableAReg++;
                        allowFReg = false;
                        memOffset += 4;

                        if(availableAReg == 4)
                            loadFromReg = false;
                    }
                }
                // load from memory
                if(useMem){
                    if(paramTypeName == "float"){
                        assemblyOut << "# (reading a floating type from memory)" << std::endl;
                        assemblyOut << "l.s $f4, " << memOffset + body->frame->getStoreSize() << "($fp)" << std::endl;
                        regToVar(assemblyOut, body->frame, "$f4", param.second);

                        // update state
                        memOffset += 4;
                    }
                    else if(paramTypeName == "double"){
                        if(memOffset % 8){
                            memOffset += 4;
                        }

                        assemblyOut << "# (reading a double type from memory)" << std::endl;
                        assemblyOut << "l.d $f4, " << memOffset + body->frame->getStoreSize() << "($fp)" << std::endl;
                        regToVar(assemblyOut, body->frame, "$f4", param.second);

                        // update state
                        memOffset += 8;
                    }
                    else{
                        assemblyOut << "# (reading a integer type from memory)" << std::endl;
                        assemblyOut << "lw $t0, " << memOffset + body->frame->getStoreSize() << "($fp)" << std::endl;
                        regToVar(assemblyOut, body->frame, "$t0", param.second);

                        // update state
                        memOffset += 4;
                    }
                }
                assemblyOut << "# loading parameter " << param.second << " in " << name << std::endl << std::endl;
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

AST* AST_FunDeclaration::getType(){
    return type;
}

int AST_FunDeclaration::getBytes(){
    return type->getBytes();
}

std::string AST_FunDeclaration::getTypeName(){
    return type->getTypeName();
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
        expr->generateFrames(_frame);
    }
    
    _frame->addVariable(name, type, type->getBytes());
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
        std::string varType = this->getType()->getTypeName();

        assemblyOut << std::endl << "# start " << varType << " var dec with definition " << name << std::endl;

        expr->compile(assemblyOut);

        // load top of stack into register
        if (varType == "float") {
            assemblyOut << "l.s $f4, 8($sp)" << std::endl;
            assemblyOut << "addiu $sp, $sp, 8" << std::endl;

            regToVar(assemblyOut, frame, "$f4", name);
        } else if (varType == "double") {
            assemblyOut << "l.d $f4, 8($sp)" << std::endl;
            assemblyOut << "addiu $sp, $sp, 8" << std::endl;

            regToVar(assemblyOut, frame, "$f4", name);
        } else {
            assemblyOut << "lw $t0, 8($sp)" << std::endl;
            assemblyOut << "addiu $sp, $sp, 8" << std::endl;

            regToVar(assemblyOut, frame, "$t0", name);
        }
        
        assemblyOut << "# end " << varType << " var dec with definition " << name << std::endl << std::endl;
    }
}

AST* AST_VarDeclaration::getType() {
    return type;
}

std::string AST_VarDeclaration::getName() {
    return this->name;
}

void AST_VarDeclaration::setStructName(std::string newName) {
    this->structName = newName;
}

std::string AST_VarDeclaration::getStructName() {
    return this->structName;
}

AST_VarDeclaration::~AST_VarDeclaration() {
    delete type;
    if(expr != nullptr)
        delete expr; 
}

AST_ArrayDeclaration::AST_ArrayDeclaration(AST* _type, std::string* _name) :
    type(_type),
    name(*_name)
{}

void AST_ArrayDeclaration::generateFrames(Frame* _frame){
    frame = _frame;
    type->generateFrames(_frame);
    // pointer size is always 4 bytes in a 32 bit system
    int pointer_size = 4;
    // pointer_size % 8 is too add padding after pointer.
    // this isn't useful for int's but when we need double word sized types this will save us
    // a lot of headaches.
    // no need to pad type->getType() since addVariable does that for us
    _frame->addVariable(name, type, pointer_size % 8 + type->getBytes());
    _frame->addVariable(name, type, pointer_size);
}

AST* AST_ArrayDeclaration::deepCopy(){
    AST* new_type = type->deepCopy();
    return new AST_ArrayDeclaration(new_type, &name);
}

void AST_ArrayDeclaration::compile(std::ostream &assemblyOut) {
    // get pointer to start of allocated memory space
    // always a double word away from allocated memory space
    assemblyOut << std::endl << "# start array declaration " << name << std::endl; 
    assemblyOut << "addiu $t0, $fp, -" << frame->getVarAddress(name).second - 8 << std::endl;
    regToVar(assemblyOut, frame, "$t0", name);
    assemblyOut << "# end array declaration " << name << std::endl << std::endl;
}

AST_ArrayDeclaration::~AST_ArrayDeclaration(){
    delete type;
}
