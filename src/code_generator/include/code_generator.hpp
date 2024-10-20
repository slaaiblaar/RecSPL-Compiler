// #include "automata.hpp"
// #include "pugixml.hpp"
// #include <unordered_map> // hashmap
// #include <set>
// #include "ast_node.hpp"
// #include <iostream>
// #include <string>
// #ifndef code_generator_h
// #define code_generator_h
// class Code_Generator
// {
// public:
//     Code_Generator();

//     // std::string generate(ASTNode* root);
//     std::string generate(std::shared_ptr<node> root);

// private:
//     std::unordered_map<std::string, std::string> vtable;
//     std::unordered_map<std::string, std::string> ftable;
// };
// #endif


#ifndef code_generator_h
#define code_generator_h

#include "ast_node.hpp"
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <fmt/core.h>

class Code_Generator
{
public:
    Code_Generator();

    // Main function to generate code from the AST root
    std::string generate(std::shared_ptr<node> root);
    vtable_type vtable;
    ftable_type ftable;

private:
    // Helper functions for translating different types of AST nodes
    std::string TransExp(std::shared_ptr<node> Exp, vtable_type& vtable, ftable_type& ftable, std::string& place);
    std::string TransStat(std::shared_ptr<node> Stat, vtable_type& vtable, ftable_type& ftable);
    std::string TransCond(std::shared_ptr<node> Cond, const std::string& labelTrue, const std::string& labelFalse, vtable_type& vtable, ftable_type& ftable);
    std::string TransExps(std::vector<std::shared_ptr<node>> Exps, vtable_type& vtable, ftable_type& ftable, std::vector<std::string>& args);

    std::string newVar();
    std::string newLabel();
    
    // vtable_type vtable;
    // ftable_type ftable;
};

#endif
