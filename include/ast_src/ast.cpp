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

void AST::setType(std::string newType) {
    throw std::runtime_error("AST: setType Not implemented by child class.\n");
}

AST* AST::getType(){
    std::cerr << "AST::getType: Not implemented by child class: Returning default type int" << std::endl;
    std::string typeName = "int";
    return new AST_Type(&typeName);
}

int AST::getBytes(){
    throw std::runtime_error("AST: getBytes Not implemented by child class.\n");
}

std::string AST::getTypeName(){
    std::cerr << "AST::getTypeName: Not implemented by child class: Returning default type int" << std::endl;
    return "int";
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
    if(it != variableType.end()) {
        return it->second;
    }
    else {
        return parentFrame->getVarType(variableName);
    }
}

void Frame::addVariable(const std::string &variableName, AST* type, int byteSize) {
    variableBindings[variableName] = memOcc;
    variableType[variableName] = type;
    memOcc += byteSize + byteSize%8;
}

void Frame::addFunction(const std::string &name, AST* fn){
    functions[name] = fn;
}

AST* Frame::getFunction(const std::string &name){
    auto it = functions.find(name);
    if(it != functions.end()) {
        return it->second;
    }
    else {
        return parentFrame->getFunction(name);
    }
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

void Frame::addCaseLabelValueMapping(std::string label, int value) {
    caseLabelValueMapping[label] = value;
}

std::map<std::string, int> Frame::getCaseLabelValueMapping() const {
    return caseLabelValueMapping;
}

std::pair<int, AST*> Frame::getFnInfo(){
    int i = 0;
    Frame* frame = this;
    while(frame->fn == nullptr){
        i++;
        frame = frame->parentFrame;
    }
    return {i, frame->fn};
}

std::pair<std::string, int> Frame::getStartLoopLabelName(std::ostream &assemblyOut) {
    int i = 0;
    Frame* frame = this;
    while (frame->startLoopLabelName == "") {
        // variable does not exist in current frame
        // try to find in parent frame
        i++;
        frame = frame->parentFrame;
    }

    return {frame->startLoopLabelName, i};
}

std::pair<std::string, int> Frame::getEndLoopLabelName(std::ostream &assemblyOut) {
    int i = 0;
    Frame* frame = this;
    while (frame->endLoopLabelName == "") {
        // variable does not exist in current frame
        // try to find in parent frame
        i++;
        frame = frame->parentFrame;
    }

    return {frame->endLoopLabelName, i};
}
