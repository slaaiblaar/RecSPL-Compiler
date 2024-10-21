// // #include "automata.hpp"
// // #include "pugixml.hpp"
// // #include <unordered_map> // hashmap
// // #include <set>
// // #include "ast_node.hpp"
// // #include <iostream>
// // #include <string>
// // #ifndef code_generator_h
// // #define code_generator_h
// // class Code_Generator
// // {
// // public:
// //     Code_Generator();

// //     // std::string generate(ASTNode* root);
// //     std::string generate(std::shared_ptr<node> root);

// // private:
// //     std::unordered_map<std::string, std::string> vtable;
// //     std::unordered_map<std::string, std::string> ftable;
// // };
// // #endif


// #ifndef code_generator_h
// #define code_generator_h

// #include "ast_node.hpp"
// #include <unordered_map>
// #include <string>
// #include <vector>
// #include <memory>
// #include <sstream>
// #include <fmt/core.h>

// class Code_Generator
// {
// public:
//     Code_Generator();

//     // Main function to generate code from the AST root
//     std::string generate(std::shared_ptr<node> root);
//     vtable_type vtable;
//     ftable_type ftable;

// private:
//     // Helper functions for translating different types of AST nodes
//     std::string TransExp(std::shared_ptr<node> Exp, vtable_type& vtable, ftable_type& ftable, std::string& place);
//     std::string TransStat(std::shared_ptr<node> Stat, vtable_type& vtable, ftable_type& ftable);
//     std::string TransCond(std::shared_ptr<node> Cond, const std::string& labelTrue, const std::string& labelFalse, vtable_type& vtable, ftable_type& ftable);
//     std::string TransExps(std::vector<std::shared_ptr<node>> Exps, vtable_type& vtable, ftable_type& ftable, std::vector<std::string>& args);

//     std::string newVar();
//     std::string newLabel();
    
//     // vtable_type vtable;
//     // ftable_type ftable;
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

    // Phase A: Generates intermediate code from the AST root
    std::string generate(std::shared_ptr<node> root);

    // Phase B: Generates final executable code from the intermediate code
    std::string generate_final(std::shared_ptr<node> root);

    vtable_type vtable;
    ftable_type ftable;

private:
    // Helper functions for translating different types of AST nodes (Phase A)
    std::string TransExp(std::shared_ptr<node> Exp, vtable_type& vtable, ftable_type& ftable, std::string& place);
    std::string TransStat(std::shared_ptr<node> Stat, vtable_type& vtable, ftable_type& ftable);
    std::string TransCond(std::shared_ptr<node> Cond, const std::string& labelTrue, const std::string& labelFalse, vtable_type& vtable, ftable_type& ftable);
    std::string TransExps(std::vector<std::shared_ptr<node>> Exps, vtable_type& vtable, ftable_type& ftable, std::vector<std::string>& args);

    // Handling function declarations and function bodies
    std::string TransDecl(std::shared_ptr<node> Decl, vtable_type& vtable, ftable_type& ftable);  // Translates function declarations (DECL)
    std::string TransHeader(std::shared_ptr<node> Header, vtable_type& vtable, ftable_type& ftable);  // Translates function header (HEADER)
    std::string TransBody(std::shared_ptr<node> Body, vtable_type& vtable, ftable_type& ftable);  // Translates function body (BODY)
    std::string TransLocVars(std::shared_ptr<node> LocVars, vtable_type& vtable, ftable_type& ftable);  // Translates local variables (LOCVARS)

    // Phase B: Converts intermediate code to executable code
    std::string generate_executable(const std::string& intermediate_code);
    
    // Phase B: Handles function calls and return statements in executable code
    void handle_function_call(std::stringstream& final_code, const std::string& call_instruction);
    void handle_return(std::stringstream& final_code, const std::string& return_instruction);

    // Utilities for generating temporary variables, labels, and stack frames
    std::string newVar();
    std::string newLabel();
    std::string new_frame();  // Generates a new stack frame (Phase B)
};

#endif
