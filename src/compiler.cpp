#include <iostream>
#include <ast>
#include <stdexcept>

#include "parser/parser.tab.hpp"

int main()
{
    try {

        // parse the AST
        AST *ast = parseAST();
        std::cerr << "Parsing Works!" << std::endl;

        // write MIPS assembly to stdout
        ast->compile(std::cout);
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
