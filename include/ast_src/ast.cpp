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
    memOcc += byteSize + byteSize % 8;
}

int Frame::getFrameSize() const {
    // adds offset for 2 bytes of memory related to storing previous frames information
    return memOcc + 16;
}

int Frame::getMemOcc() const {
    return memOcc;
}
