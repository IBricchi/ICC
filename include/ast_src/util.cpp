#include "util.hpp"

int getTypeByteSize(const std::string &type) {
    if (type == "int") {
        return 8;
    }

    throw std::runtime_error("getTypeByteSize: Not Implemented Yet.\n");
}

int uniqueLabelCount = 0;
std::string generateUniqueLabel(const std::string &labelName) {
    return labelName + std::to_string(uniqueLabelCount++);
}
