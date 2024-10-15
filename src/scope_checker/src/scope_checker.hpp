// scope_checker.hpp
#ifndef SCOPE_CHECKER_HPP
#define SCOPE_CHECKER_HPP

#include <iostream>
#include <unordered_map>
#include <stack>
#include <string>
#include <vector>

class Scope_Checker {
public:
    Scope_Checker();  

    void pushScope();

    void popScope();

    bool isVariableDeclared(const std::string &varName);

    bool isFunctionDeclared(const std::string &funcName);

    void addVariable(const std::string &varName, const std::string &type);

    void addFunction(const std::string &funcName, const std::string &returnType);

    void testScopeChecker();

private:
    // Structure for storing variable and function information
    struct SymbolTable {
        std::unordered_map<std::string, std::string> variables; // variable name -> type
        std::unordered_map<std::string, std::string> functions; // function name -> return type
    };

    // Global stack for scopes
    std::stack<SymbolTable> scopeStack;

    // Random variable and function generators
    void generateRandomVar();
    void generateRandomFunc();
    void generateRandomOp();
    void generateRandomProgram();
};

#endif
