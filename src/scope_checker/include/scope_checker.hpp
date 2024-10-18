// #ifndef scope_checker_h
// #define scope_checker_h

// class Scope_Checker
// {
// public:
//     Scope_Checker();

// private:
// };
// #endif

#ifndef scope_checker_h
#define scope_checker_h

#include <iostream>
#include <unordered_map>
#include <stack>
#include <string>
#include <vector>
#include <memory>
#include <pugixml.hpp>
#include <set>
#include <fmt/core.h>
#include "ast_node.hpp"
class Scope_Checker
{
public:
    Scope_Checker(std::string cfg_file = "CFG.xml");

    void pushScope();
    void popScope();
    bool isVariableDeclared(const std::string &varName);
    bool isFunctionDeclared(const std::string &funcName);
    void addVariable(const std::string &varName, const std::string &type);
    void addFunction(const std::string &funcName, const std::string &returnType);
    void testScopeChecker(int);
    std::string cfg_file = "CFG.xml";

private:
    struct SymbolTable
    {
        std::unordered_map<std::string, std::string> variables; // variable name -> type
        std::unordered_map<std::string, std::string> functions; // function name -> return type
    };

    std::stack<SymbolTable> scopeStack;

    void generateRandomVar();
    void generateRandomFunc();
    void generateRandomOp();
    void generateRandomProgram();
};

void generate_tree(std::shared_ptr<node> n, pugi::xml_node productions);

#endif
