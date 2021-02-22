#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <map>

class AST;

class AST
{
public:
    virtual std::string compile() = 0;
};