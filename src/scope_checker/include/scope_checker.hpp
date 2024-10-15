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
    struct SymbolTable {
        std::unordered_map<std::string, std::string> variables; // variable name -> type
        std::unordered_map<std::string, std::string> functions; // function name -> return type
    };

    std::stack<SymbolTable> scopeStack;

    void generateRandomVar();
    void generateRandomFunc();
    void generateRandomOp();
    void generateRandomProgram();
};

class node {
public:
    std::string NAME;   // Leaf or Internal node
    int UID;            // Unique identifier
    std::string CLASS;  // Class of the node (nonterminal or terminal type)
    std::string WORD;   // Word representing the production
    std::vector<std::shared_ptr<node>> children;  // Child nodes

    node();  
};

void generate_tree(std::shared_ptr<node> n, pugi::xml_node productions);

#endif
