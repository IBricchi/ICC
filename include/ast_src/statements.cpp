#include "statement.hpp"

AST_Statement::~AST_Statement() {
}

AST_Return::AST_Return(AST* _expr = nullptr) :
    expr(_expr)
{}

void AST_Return::compile(std::ostream &assemblyOut) {
    if (expr == nullptr) {
        // return 0 by default
        assemblyOut << "addiu $v0, $0, $0" << endl;
    }
    else {
        // Need to first evaluate expression (likely multiple assembly lines)
        // and then somehow put the final value into register $v0.
        // How do we find out what the final value is? Keep track of last register that we stored something in? Some sort of context?
        // Can probably use the same idea as for loading variables into registers and then using that register in future assembly.

        throw std::runtime_error("Not Implemented Yet.\n");
    }

    // Need to do stuff for ending function call.
    // E.g. moving stack and frame pointers
    // See lecture 10 for help
}

AST_Return::~AST_Return() {
    delete expr;
}

AST_IfStmt::AST_IfStmt(AST* _cond, AST* _then, AST* _other = nullptr) :
    cond(_cond),
    then(_then),
    other(_other)
{}

void AST_IfStmt::compile(std::ostream &assemblyOut) {
    throw std::runtime_error("Not Implemented Yet.\n");
}

AST_IfStmt::~AST_IfStmt(){
    delete cond;
    delete then;
    delete other;
}
