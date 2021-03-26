#pragma once

#include <string>
#include "ast.hpp"
#include "expression.hpp"

class AST_Sequence
    : public AST
{
private:
    AST* first;
    AST* second;

    // Used for struct
    std::string structName;

public:
    AST_Sequence(AST* _first, AST* _second);

    void generateFrames(Frame* _frame = nullptr) override;
    AST* deepCopy() override;
    void compile(std::ostream &assemblyOut) override;

    void setStructName(std::string newName) override;
    std::string getStructName() override;

    ~AST_Sequence();
};

class AST_FunDeclaration
    : public AST
{
private:
    AST* type;
    std::string name;
    AST* body;
    int parity;
    // first in params is type, second is variable name
    std::vector<std::pair<AST*, std::string>>* params;

public:
    /*
        Function body is optional and can be provided in a function definition later on.
    */
    AST_FunDeclaration(AST* type, std::string* _name, AST* _body = nullptr, std::vector<std::pair<AST*,std::string>>* _params = nullptr);

    void generateFrames(Frame* _frame = nullptr) override;
    AST* deepCopy() override;
    void compile(std::ostream &assemblyOut) override;

    AST* getType() override;
    int getBytes() override;
    std::string getTypeName() override;

    ~AST_FunDeclaration();
};

/*
    Corresponding object is AST_Variable in primitive.hpp.
*/
class AST_VarDeclaration
    : public AST
{
private:
    AST* type;
    std::string name;
    AST* expr;

    // Used for struct
    std::string structName;
    std::map<std::string, std::string> structAttributeNameTypeMap;

public:
    AST_VarDeclaration(AST* _type, std::string* _name, AST* _expr = nullptr);

    // Used for struct
    AST_VarDeclaration(AST* _type, std::string* _name, const std::map<std::string, std::string> &_structAttributeNameTypeMap);

    void generateFrames(Frame* _frame = nullptr) override;
    AST* deepCopy() override;
    void compile(std::ostream &assemblyOut) override;
    AST* getType() override;

    std::string getName() override;

    void setStructName(std::string newName) override;
    std::string getStructName() override;

    ~AST_VarDeclaration();
};

class AST_ArrayDeclaration
    : public AST
{
private:
    AST* type;
    std::string name;

    std::vector<AST*>* initializerList1D;
    std::vector<std::vector<AST*>*>* initializerList2D;
public:
    AST_ArrayDeclaration(AST* _type, std::string* _name);

    // 1D array initializer list
    AST_ArrayDeclaration(AST* _type, std::string* _name, std::vector<AST*>* initializerList);

    // 2D array initializer list
    AST_ArrayDeclaration(AST* _type, std::string* _name, std::vector<std::vector<AST*>*>* initializerList);

    void generateFrames(Frame* _frame = nullptr) override;
    AST* deepCopy() override;
    void compile(std::ostream &assemblyOut) override;
    AST* getType() override;

    std::string getName() override;

    ~AST_ArrayDeclaration();
};
