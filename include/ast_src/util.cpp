#include "util.hpp"

int getTypeByteSize(const std::string &type) {
    if (type == "int") {
        return 8;
    }

    throw std::runtime_error("getTypeByteSize: Not Implemented Yet.\n");
}

std::string generateUniqueLabel(const std::string &labelName) {
    static int uniqueLabelCount = 0;
    return labelName + std::to_string(uniqueLabelCount++);
}

void regToVar(std::ostream &assemblyOut, Frame* frame, const std::string& reg, const std::string& var){
    std::pair<int, int> varAddress = frame->getVarAddress(var);
    
    // coppy frame pointer to t1 and recurse back expected number of frames
    assemblyOut << "move $t6, $fp" << std::endl;
    for(int i = 0; i < varAddress.first; i++){
        assemblyOut << "lw $t6, 12($t6)" << std::endl;
    }
    
    // store register data into variable's memory address
    assemblyOut << "sw " << reg << ", -" << varAddress.second << "($t6)" << std::endl;
}

void varToReg(std::ostream &assemblyOut, Frame* frame, const std::string& reg, const std::string& var){
    std::pair<int, int> varAddress = frame->getVarAddress(var);
    
    // coppy frame pointer to t1 and recurse back expected number of frames
    assemblyOut << "move $t6, $fp" << std::endl;
    for(int i = 0; i < varAddress.first; i++){
        assemblyOut << "lw $t6, 12($t6)" << std::endl;
    }
    
    // store register data into variable's memory address
    assemblyOut << "lw " << reg << ", -" << varAddress.second << "($t6)" << std::endl;
}
