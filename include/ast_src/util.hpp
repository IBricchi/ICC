#pragma once

#include "ast.hpp"

// General utility functions

// Undefined behaviour but works on the environment defined by the Vagrant file and g++ compiler
union {
    uint32_t num;
    float fnum;
} ieee754Float;
union {
    uint64_t num;
    double dnum;
} ieee754Double;

std::string generateUniqueLabel(const std::string &labelName);

// uses t6 as temporary
void regToVar(std::ostream &assemblyOut, Frame* frame, const std::string& reg, const std::string& var, const std::string& reg_2 = "");

// uses t6 as temporary
void varToReg(std::ostream &assemblyOut, Frame* frame, const std::string& reg, const std::string& var);
// uses t6 as temporary
void varAddressToReg(std::ostream &assemblyOut, Frame* frame, const std::string& reg, const std::string& var);

// for global variables
void valueToVarLabel(std::ostream &assemblyOut, std::string varLabel, int value);
void valueToVarLabel(std::ostream &assemblyOut, std::string varLabel, float value);
void valueToVarLabel(std::ostream &assemblyOut, std::string varLabel, double value);

// check if string ends with suffix
bool hasEnding(const std::string &fullString, const std::string &ending);
