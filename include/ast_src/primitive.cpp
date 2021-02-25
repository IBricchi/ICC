#include "primitive.hpp"

AST_ConstInt::AST_ConstInt(int _value):
    value(_value)
{}

void AST_ConstInt::generateFrames(Frame* _frame){
    frame = _frame;
}

void AST_ConstInt::compile(std::ostream &assemblyOut){
    throw std::runtime_error("AST_ConstInt: Not implemented Yet.\n");
}

AST_Constant::AST_Constant(AST* _value) :
    value(_value)
{}

void AST_Constant::generateFrames(Frame* _frame){
    frame = _frame;
}

void AST_Constant::compile(std::ostream &assemblyOut) {
    // I don't think that will be enough for the assembly
    assemblyOut << value;
}

AST_Constant::~AST_Constant() {
     delete value;
 }

AST_Variable::AST_Variable(std::string* _name) :
    name(*_name)
{}

void AST_Variable::generateFrames(Frame* _frame){
    frame = _frame;
}

void AST_Variable::compile(std::ostream &assemblyOut) {
    /*
        Need to load variable from memory into 'unused' register.
        Somehow need to make this register known to caller of this method so that can be used in future assembly.
        If using register, how do we know when a register becomes free again without doing an initial register
        allocation?

        See AST_VarDeclaration declaration in structure.hpp for more context.

        IBricchi:
        ---------
        I think this problem is sovled using the frames we just implemented
    */
    throw std::runtime_error("AST_Variable: Not Implemented Yet.\n");
}
