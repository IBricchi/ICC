#include<iostream>
#include<ast>
#include <stdexcept>

int main()
{
	

    // general error catcher
    catch(std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
