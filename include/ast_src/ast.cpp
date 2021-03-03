#include "ast.hpp"

void AST::generateFrames(Frame* _frame){
    throw std::runtime_error("AST: Not implemented yet by child class.\n");
}

void AST::compile(std::ostream &assemblyOut) {
    throw std::runtime_error("AST: Not implemented yet by child class.\n");
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

int Frame::getMemoryAddress(const std::string &variableName) const {
    // first try to find in current frame
    auto variableBinding = variableBindings.find(variableName);
    if (variableBinding != variableBindings.end()) {
        return variableBinding->second;
    }
    
    // variable does not exist in current frame
    // try to find in parent frame
    return parentFrame->getMemoryAddress(variableName);
}

void Frame::addVariable(const std::string &variableName, int byteSize) {
    variableBindings[variableName] = memOcc;
    memOcc += byteSize * 4;
}

int Frame::getStoreSize() const {
    return storeSize;
}

int Frame::getVarSize() const {
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
