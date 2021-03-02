#pragma once

#include "ast.hpp"

// General utility functions

int getTypeByteSize(const std::string &type);

extern int uniqueLabelCount;
std::string generateUniqueLabel(const std::string &labelName);
