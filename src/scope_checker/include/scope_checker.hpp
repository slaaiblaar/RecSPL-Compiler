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

    // Push a new scope onto the stack
    void pushScope();

    // Pop the current scope from the stack
    void popScope();

    // Check if a variable is declared in any scope
    bool isVariableDeclared(const std::string &varName);

    // Check if a function is declared
    bool isFunctionDeclared(const std::string &funcName);

    // Add a variable to the current scope
    void addVariable(const std::string &varName, const std::string &type);

    // Add a function to the current scope
    void addFunction(const std::string &funcName, const std::string &returnType);

    // Test the scope checker using random AST generation
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

class node {
public:
    std::string NAME;   // Leaf or Internal node
    int UID;            // Unique identifier
    std::string CLASS;  // Class of the node (nonterminal or terminal type)
    std::string WORD;   // Word representing the production
    std::vector<std::shared_ptr<node>> children;  // Child nodes

    node();  // Constructor
};

// Function to recursively generate an AST
void generate_tree(std::shared_ptr<node> n, pugi::xml_node productions);

#endif
