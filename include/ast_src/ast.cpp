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
    memOcc += byteSize;
}

int Frame::getFrameSize() const {
    // Trivial static implementation

    // Ensure that value is doubleword aligned
    //return (memOcc % 8 == 0) ? (memOcc + 25*8) : (memOcc + 25*8 + 4);

    /*
        Trivial solution => Allocate more space than ever needed (very inefficient)

        Avoids the need of keeping track of initial frame size for use in ASTReturn (popping frame of stack)
            => assemblyOut << "addiu $sp, $sp, " << frame->getFrameSize() << std::endl;
    */
    return 25*8;
}

int Frame::getMemOcc() const {
    return memOcc;
}

int Frame::setLoopLabelNames(std::string _startLoopLabelName, std::string _endLoopLabelName) {
    startLoopLabelName = _startLoopLabelName;
    endLoopLabelName = _endLoopLabelName;
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
