#include "ast.hpp"

void AST::generateFrames(Frame* _frame){
    throw std::runtime_error("AST: generateFrames Not implemented yet by child class.\n");
}

void AST::compile(std::ostream &assemblyOut) {
    throw std::runtime_error("AST: compile Not implemented yet by child class.\n");
}

void AST::updateVariable(std::ostream &assemblyOut, Frame* currentFrame, std::string reg) {
    throw std::runtime_error("AST: updateVariable Not implemented by child class.\n");
}

AST* AST::deepCopy(){
    throw std::runtime_error("AST: deepCopy Not implemented by child class.\n");
}

AST* AST::getType(){
    throw std::runtime_error("AST: getType Not implemented by child class.\n");
}

int AST::getBytes(){
    throw std::runtime_error("AST: getBytes Not implemented by child class.\n");
}

AST::~AST() {
    delete frame;
}

Frame::Frame(Frame* _parentFrame) :
    parentFrame(_parentFrame)
{}

Frame::~Frame() {
    delete parentFrame;
}

int Frame::getVarPos(const std::string &variableName) const{
    auto variableBinding = variableBindings.find(variableName);
    if (variableBinding != variableBindings.end()) {
        return variableBinding->second;
    }
    return -1;
}

std::pair<int, int> Frame::getVarAddress(const std::string &variableName) {
    int depth = 0;
    Frame* frame = this;
    int pos = frame->getVarPos(variableName);
    while(pos == -1){
        depth++;
        frame = frame->parentFrame;
        pos = frame->getVarPos(variableName);
    }
    return {depth, pos};
}

AST* Frame::getVarType(const std::string& variableName) const{
    auto it = variableType.find(variableName);
    if(it != variableType.end())
        return it->second;
    else
        return parentFrame->getVarType(variableName);
}

void Frame::addVariable(const std::string &variableName, AST* type, int byteSize) {
    variableBindings[variableName] = memOcc;
    variableType[variableName] = type;
    memOcc += byteSize + byteSize%8;
}

int Frame::getStoreSize() const {
    return storeSize;
}

int Frame::getVarStoreSize() const {
    return memOcc;
}

void Frame::setLoopLabelNames(std::string _startLoopLabelName, std::string _endLoopLabelName) {
    startLoopLabelName = _startLoopLabelName;
    endLoopLabelName = _endLoopLabelName;
}

int Frame::getDistanceToFun(){
    int i = 0;
    Frame* frame = this;
    while(!frame->isFun){
        i++;
        frame = frame->parentFrame;
    }
    return i;
}

std::string Frame::getStartLoopLabelName() const {
    // first try to find in current frame
    if (startLoopLabelName != "") {
        return startLoopLabelName;
    }
    
    // variable does not exist in current frame
    // try to find in parent frame
    return parentFrame->getStartLoopLabelName();
}
std::string Frame::getEndLoopLabelName() const {    
    // first try to find in current frame
    if (endLoopLabelName != "") {
        return endLoopLabelName;
    }
    
    // variable does not exist in current frame
    // try to find in parent frame
    return parentFrame->getEndLoopLabelName();
}
