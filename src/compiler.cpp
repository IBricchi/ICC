#include <iostream>
#include <ast>
#include <stdexcept>

#include "parser/parser.tab.hpp"

void printAssemblyHeader(std::ostream &assemblyOut) {
    assemblyOut << ".section .mdebug.abi32" << std::endl;
    assemblyOut << ".previous" << std::endl;
    assemblyOut << ".nan	legacy" << std::endl;
    assemblyOut << ".module	fp=32" << std::endl;
    assemblyOut << ".module	oddspreg" << std::endl;
    assemblyOut << ".abicalls" << std::endl;
    assemblyOut << ".text" << std::endl;
}

void printAssemblyFooter(std::ostream &assemblyOut) {
    assemblyOut << ".ident	\"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.9) 5.4.0 20160609\"" << std::endl;
}

int main()
{
    try {

        // parse the AST
        AST *ast = parseAST();
        std::cerr << "Parsing Works!" << std::endl;

        // global frame
        Frame* globalFrame = new Frame();

        // pre-process AST to generate Frame objects
        ast->generateFrames(globalFrame);
        std::cerr << "Frame Generation Works!" << std::endl;

        // write MIPS assembly to stdout
        printAssemblyHeader(std::cout);
        ast->compile(std::cout);
        printAssemblyFooter(std::cout);
        std::cerr << "Compiling Works!" << std::endl;
    }
    
    // general exception handler
    catch(std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    catch(...) {
        std::cerr << "UNKNOWN ERROR" << std::endl;
        exit(EXIT_FAILURE);
    }
}
