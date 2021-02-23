#include "ast.hpp"

AST::~AST() {
}

void AST::compile(std::ostream &assemblyOut, Frame &frame) {
    throw std::runtime_error("Not implemented yet by child class.\n");
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
