#include <iostream>
#include <ast>
#include <stdexcept>

#include "parser/parser.tab.hpp"

int main()
{
    try {

        // parse the AST
        AST *ast = parseAST();

        std::cout << "Parsing Works!" << std::endl;

        // write MIPS assembly to stdout
        ast->compile(std::cout);
    }
    
    // general error catcher
    catch(std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
