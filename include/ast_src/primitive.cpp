#include "primitive.hpp"

AST_Constant::~AST_Primitive() {
}

AST_Constant::AST_Constant(AST* _value) :
    value(_value)
{}

void AST_Constant::compile(std::ostream &assemblyOut, Frame &frame) {
    assemblyOut << value;
}

AST_Constant::~AST_Constant() {
     delete value;
 }

AST_Variable::AST_Variable(AST* _name) :
    name(_name)
{}

void AST_Variable::compile(std::ostream &assemblyOut, Frame &frame) {
    /*
        Need to load variable from memory into 'unused' register.
        Somehow need to make this register known to caller of this method so that can be used in future assembly.
        If using register, how do we know when a register becomes free again without doing an initial register
        allocation?

        See AST_VarDeclaration declaration in structure.hpp for more context.
    */
    throw std::runtime_error("Not Implemented Yet.\n");
}

AST_Variable::~AST_Variable() {
    delete name;
}
