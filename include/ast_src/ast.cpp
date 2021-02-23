#include "ast.hpp"

AST::~AST() {
}

Frame::Frame(Frame _parentFrame) :
    parentFrame(_parentFrame)
{}

Frame::~Frame() {
    delete parentFrame;
}

int Frame::getMemoryAddress(const string &variableName) {
    // first try to find in current frame
    auto variableBinding = variableBindings.find(variableName);
    if (variableBinding != variableBindings.end()) {
        return variableBinding->second;
    }
    
    // variable does not exist in current frame
    // try to find in parent frame
    return parentFrame->getMemoryAddress(variableName);
}

void Frame::addVariable(string variableName, int memAddress) {
    variableBindings[variableName] = memAddress;
}
