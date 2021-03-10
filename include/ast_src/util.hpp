#pragma once

#include "ast.hpp"

// General utility functions

int getTypeByteSize(const std::string &type);

std::string generateUniqueLabel(const std::string &labelName);

// uses t6 as temporary
void regToVar(std::ostream &assemblyOut, Frame* frame, const std::string& reg, const std::string& var);
// uses t6 as temporary
void varToReg(std::ostream &assemblyOut, Frame* frame, const std::string& reg, const std::string& var);
// uses t6 as temporary
void varAddressToReg(std::ostream &assemblyOut, Frame* frame, const std::string& reg, const std::string& var);
