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

    sym_table_type vtable;
    sym_table_type ftable;

// private:
    // Helper functions for translating different types of AST nodes (Phase A)
    std::string TransExp(std::shared_ptr<node> Exp, sym_table_type& vtable, sym_table_type& ftable, std::string& place);
    std::string TransStat(std::shared_ptr<node> Stat, sym_table_type& vtable, sym_table_type& ftable);
    std::string TransCond(std::shared_ptr<node> Cond, const std::string& labelTrue, const std::string& labelFalse, sym_table_type& vtable, sym_table_type& ftable);
    std::string TransExps(std::vector<std::shared_ptr<node>> Exps, sym_table_type& vtable, sym_table_type& ftable, std::vector<std::string>& args);

    // Handling function declarations and function bodies
    std::string TransDecl(std::shared_ptr<node> Decl, sym_table_type& vtable, sym_table_type& ftable);
    std::string TransHeader(std::shared_ptr<node> Header, sym_table_type& vtable, sym_table_type& ftable);
    std::string TransBody(std::shared_ptr<node> Body, sym_table_type& vtable, sym_table_type& ftable);
    std::string TransLocVars(std::shared_ptr<node> LocVars, sym_table_type& vtable, sym_table_type& ftable);

    // New functions for handling higher-level constructs
    // std::string TransProg(std::shared_ptr<node> Prog, sym_table_type& vtable, sym_table_type& ftable);
    // std::string TransGlobVars(std::shared_ptr<node> GlobVars, sym_table_type& vtable);
    // std::string TransFunctions(std::shared_ptr<node> Functions, sym_table_type& vtable, sym_table_type& ftable);
    // std::string TransFunction(std::shared_ptr<node> Function, sym_table_type& vtable, sym_table_type& ftable);
    // std::string TransAlgo(std::shared_ptr<node> Algo, sym_table_type& vtable, sym_table_type& ftable);

    // Phase B: Converts intermediate code to executable code
    std::string generate_executable(const std::string& intermediate_code);
    
    // Phase B: Handles function calls and return statements in executable code
    void handle_function_call(std::stringstream& final_code, const std::string& call_instruction);
    void handle_return(std::stringstream& final_code, const std::string& return_instruction);

    // Utilities for generating temporary variables, labels, and stack frames
    std::string newVar();
    std::string newLabel();
    std::string new_frame();  // Generates a new stack frame (Phase B)

    // //Helper functions
    // bool hasChild(const std::shared_ptr<node>& n, const std::string& child_class);
    // std::shared_ptr<node> getChild(const std::shared_ptr<node>& n, size_t index);
    
};

#endif
