#include<iostream>
#include <ast>
#include <stdexcept>

int main()
{
    try {

        // parse the AST
        AST *ast = parseAST();

        std::string mipsAssembly = ast->compile();

    }
    
    // general error catcher
    catch(std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
