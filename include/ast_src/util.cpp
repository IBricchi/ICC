#include "util.hpp"

int getTypeByteSize(std::string type) {
    if (type == "int") {
        return 8;
    }

    throw std::runtime_error("getTypeByteSize: Not Implemented Yet.\n");
}
