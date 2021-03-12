#include "util.hpp"

std::string generateUniqueLabel(const std::string &labelName) {
    static int uniqueLabelCount = 0;
    return labelName + std::to_string(uniqueLabelCount++);
}

void regToVar(std::ostream &assemblyOut, Frame* frame, const std::string& reg, const std::string& var){
    std::pair<int, int> varAddress = frame->getVarAddress(var);
    std::string varType = frame->getVarType(var)->getTypeName();
    
    // coppy frame pointer to t6 and recurse back expected number of frames
    assemblyOut << "move $t6, $fp" << std::endl;
    for(int i = 0; i < varAddress.first; i++){
        assemblyOut << "lw $t6, 12($t6)" << std::endl;
    }
    
    // store register data into variable's memory address
    if (varType == "float") {
        assemblyOut << "s.s " << reg << ", -" << varAddress.second << "($t6)" << std::endl;
    } else {
        assemblyOut << "sw " << reg << ", -" << varAddress.second << "($t6)" << std::endl;
    }
}

void varToReg(std::ostream &assemblyOut, Frame* frame, const std::string& reg, const std::string& var){
    std::pair<int, int> varAddress = frame->getVarAddress(var);
    std::string varType = frame->getVarType(var)->getTypeName();
    
    // coppy frame pointer to t6 and recurse back expected number of frames
    assemblyOut << "move $t6, $fp" << std::endl;
    for(int i = 0; i < varAddress.first; i++){
        assemblyOut << "lw $t6, 12($t6)" << std::endl;
    }
    
    // load from memory into register
    if (varType == "float") {
        assemblyOut << "l.s " << reg << ", -" << varAddress.second << "($t6)" << std::endl;
    } else {
        assemblyOut << "lw " << reg << ", -" << varAddress.second << "($t6)" << std::endl;
    }
}

void varAddressToReg(std::ostream &assemblyOut, Frame* frame, const std::string& reg, const std::string& var){
    std::pair<int, int> varAddress = frame->getVarAddress(var);
    
    // coppy frame pointer to t6 and recurse back expected number of frames
    assemblyOut << "move $t6, $fp" << std::endl;
    for(int i = 0; i < varAddress.first; i++){
        assemblyOut << "lw $t6, 12($t6)" << std::endl;
    }
    
    // store variable address into register
    assemblyOut << "addiu " << reg << ", $t6, -" << varAddress.second << std::endl;
}

bool hasEnding(const std::string &fullString, const std::string &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}
