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
    Scope_Checker(std::shared_ptr<node> root, std::string cfg_file = "CFG.xml");
    ~Scope_Checker() { root->clear_node(); }
    void pushScope();
    void popScope();
    bool isVariableDeclared(const std::string &varName);
    bool isFunctionDeclared(const std::string &funcName);
    void addVariable(const std::string &varName, const std::string &type);
    void addFunction(const std::string &funcName, const std::string &returnType);
    void testScopeChecker(int);
    bool check(std::shared_ptr<node> n, int depth = 0);
    bool run_scope_checker(int thread_number = -1);
    std::shared_ptr<sym_table_type> preprocess_ftables(std::shared_ptr<node> n, int depth);
    void construct_ftables(std::shared_ptr<node> n, int depth);
    void populate_identifiers(std::shared_ptr<node> n, int depth = 0);
    std::string cfg_file = "CFG.xml";
    std::shared_ptr<node> root;
    std::vector<std::pair<std::string, std::pair<int, int>>> error;
    std::vector<std::pair<std::string, std::shared_ptr<node>>> error_messages;

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
