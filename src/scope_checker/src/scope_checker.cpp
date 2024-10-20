// // scope_checker.cpp
// #include "scope_checker.hpp"
// #include <cstdlib>
// #include <ctime>

// // Constructor
// Scope_Checker::Scope_Checker() {
//     std::cout << "Scope Checker initialized\n";
// }

// void Scope_Checker::pushScope() {
//     scopeStack.push(SymbolTable());
// }

// void Scope_Checker::popScope() {
//     if (!scopeStack.empty()) {
//         scopeStack.pop();
//     }
// }

// bool Scope_Checker::isVariableDeclared(const std::string &varName) {
//     std::stack<SymbolTable> tempStack = scopeStack;
//     while (!tempStack.empty()) {
//         if (tempStack.top().variables.find(varName) != tempStack.top().variables.end()) {
//             return true;
//         }
//         tempStack.pop();
//     }
//     return false;
// }

// bool Scope_Checker::isFunctionDeclared(const std::string &funcName) {
//     std::stack<SymbolTable> tempStack = scopeStack;
//     while (!tempStack.empty()) {
//         if (tempStack.top().functions.find(funcName) != tempStack.top().functions.end()) {
//             return true;
//         }
//         tempStack.pop();
//     }
//     return false;
// }

// void Scope_Checker::addVariable(const std::string &varName, const std::string &type) {
//     if (!scopeStack.empty()) {
//         scopeStack.top().variables[varName] = type;
//     }
// }

// void Scope_Checker::addFunction(const std::string &funcName, const std::string &returnType) {
//     if (!scopeStack.empty()) {
//         scopeStack.top().functions[funcName] = returnType;
//     }
// }

// // Random variable and function generation for testing
// const std::vector<std::string> varTypes = {"num", "text"};
// const std::vector<std::string> varNames = {"V_a", "V_b", "V_c", "V_x", "V_y", "V_z"};
// const std::vector<std::string> funcNames = {"F_add", "F_sub", "F_mul", "F_div"};
// const std::vector<std::string> operations = {"add", "sub", "mul", "div"};

// void Scope_Checker::generateRandomVar() {
//     std::string varType = varTypes[std::rand() % varTypes.size()];
//     std::string varName = varNames[std::rand() % varNames.size()];
//     std::cout << "Declaring variable " << varName << " of type " << varType << "\n";
//     addVariable(varName, varType);
// }

// void Scope_Checker::generateRandomFunc() {
//     std::string funcName = funcNames[std::rand() % funcNames.size()];
//     std::string returnType = varTypes[std::rand() % varTypes.size()];
//     std::cout << "Declaring function " << funcName << " with return type " << returnType << "\n";
//     addFunction(funcName, returnType);
//     pushScope();
//     for (int i = 0; i < 3; ++i) generateRandomVar();  // Add local variables
//     popScope();
// }

// void Scope_Checker::generateRandomOp() {
//     std::string op = operations[std::rand() % operations.size()];
//     std::string var1 = varNames[std::rand() % varNames.size()];
//     std::string var2 = varNames[std::rand() % varNames.size()];
//     std::cout << "Performing operation: " << op << "(" << var1 << ", " << var2 << ")\n";
//     if (!isVariableDeclared(var1) || !isVariableDeclared(var2)) {
//         std::cout << "Error: One or both variables not declared\n";
//     }
// }

// void Scope_Checker::generateRandomProgram() {
//     pushScope();  // Global scope

//     // Generate global variables
//     for (int i = 0; i < 2; ++i) generateRandomVar();

//     // Generate functions
//     for (int i = 0; i < 2; ++i) generateRandomFunc();

//     // Generate operations and scope checks
//     for (int i = 0; i < 3; ++i) generateRandomOp();

//     popScope();  // End global scope
// }

// void Scope_Checker::testScopeChecker() {
//     std::cout << "===== Running Random Program Test =====\n";
//     generateRandomProgram();
//     std::cout << "===== Test Complete =====\n\n";
// }

// int main() {
//     std::srand(std::time(0));  // Seed the random generator

//     Scope_Checker scopeChecker;

//     // Run tests
//     for (int i = 0; i < 5; ++i) {
//         scopeChecker.testScopeChecker();
//     }

//     return 0;
// }

#include <fmt/core.h>
#include "scope_checker.hpp"
#include "automata.hpp"
#include "pugixml.hpp"
#include <cstdlib>
#include <ctime>
#include <fstream>

// int node_counter = 0; // Global UID counter

Scope_Checker::Scope_Checker(std::shared_ptr<node> root, std::string cfg_file) : root(root), cfg_file(cfg_file)
{
    // std::cout << "Scope Checker initialized\n";
}

void Scope_Checker::pushScope()
{
    scopeStack.push(SymbolTable());
}

void Scope_Checker::popScope()
{
    if (!scopeStack.empty())
    {
        scopeStack.pop();
    }
}

bool Scope_Checker::isVariableDeclared(const std::string &varName)
{
    std::stack<SymbolTable> tempStack = scopeStack;
    while (!tempStack.empty())
    {
        if (tempStack.top().variables.find(varName) != tempStack.top().variables.end())
        {
            return true;
        }
        tempStack.pop();
    }
    return false;
}

bool Scope_Checker::isFunctionDeclared(const std::string &funcName)
{
    std::stack<SymbolTable> tempStack = scopeStack;
    while (!tempStack.empty())
    {
        if (tempStack.top().functions.find(funcName) != tempStack.top().functions.end())
        {
            return true;
        }
        tempStack.pop();
    }
    return false;
}

void Scope_Checker::addVariable(const std::string &varName, const std::string &type)
{
    if (!scopeStack.empty())
    {
        scopeStack.top().variables[varName] = type;
    }
}

void Scope_Checker::addFunction(const std::string &funcName, const std::string &returnType)
{
    if (!scopeStack.empty())
    {
        scopeStack.top().functions[funcName] = returnType;
    }
}

// Random variable and function generation for testing
const std::vector<std::string> varTypes = {"num", "text"};
const std::vector<std::string> varNames = {"V_a", "V_b", "V_c", "V_x", "V_y", "V_z"};
const std::vector<std::string> funcNames = {"F_add", "F_sub", "F_mul", "F_div"};
const std::vector<std::string> operations = {"add", "sub", "mul", "div"};

void Scope_Checker::generateRandomVar()
{
    std::string varType = varTypes[std::rand() % varTypes.size()];
    std::string varName = varNames[std::rand() % varNames.size()];
    std::cout << "Declaring variable " << varName << " of type " << varType << "\n";
    addVariable(varName, varType);
}

void Scope_Checker::generateRandomFunc()
{
    std::string funcName = funcNames[std::rand() % funcNames.size()];
    std::string returnType = varTypes[std::rand() % varTypes.size()];
    std::cout << "Declaring function " << funcName << " with return type " << returnType << "\n";
    addFunction(funcName, returnType);
    pushScope();
    for (int i = 0; i < 3; ++i)
        generateRandomVar(); // Add local variables
    popScope();
}

void Scope_Checker::generateRandomOp()
{
    std::string op = operations[std::rand() % operations.size()];
    std::string var1 = varNames[std::rand() % varNames.size()];
    std::string var2 = varNames[std::rand() % varNames.size()];
    std::cout << "Performing operation: " << op << "(" << var1 << ", " << var2 << ")\n";
    if (!isVariableDeclared(var1) || !isVariableDeclared(var2))
    {
        std::cout << "Error: One or both variables not declared\n";
    }
}

void Scope_Checker::generateRandomProgram()
{
    pushScope();

    // Generate global variables
    for (int i = 0; i < 2; ++i)
        generateRandomVar();

    // Generate functions
    for (int i = 0; i < 2; ++i)
        generateRandomFunc();

    // Generate operations and scope checks
    for (int i = 0; i < 3; ++i)
        generateRandomOp();

    popScope();
}
std::shared_ptr<ftable_type> Scope_Checker::preprocess_ftables(std::shared_ptr<node> n, int depth)
{
    // std::cout << fmt::format("\033[34m{}\033[0mPreprocessing {} {}\n", std::string(depth * 2, ' '), n->WORD, n->UID);
    ++depth;
    std::shared_ptr<node> header = n->get_child(0)->get_child(0);
    if (header->WORD != "HEADER")
    {
        std::cerr << "\nFUNCTIONS->children[0]->children[0] is not a HEADER node\n";
    }
    std::shared_ptr<node> fname = header->get_child(1);
    if (fname->WORD != "FNAME")
    {
        std::cerr << "\nHEADER->children[1] is supposed to be a FNAME node\n";
    }
    std::shared_ptr<node> fid = fname->get_child(0);
    if (fid->CLASS != "FID")
    {
        std::cerr << "\nFNAME->children[1] is supposed to be a FID node\n";
    }
    // if scope collision
    if (n->scope_f_table->find(fid->WORD) != n->scope_f_table->end())
    {
        error.push_back(std::pair<std::string, std::pair<int, int>>(fid->WORD, std::pair<int, int>(-1, -1)));
        error_messages.push_back(std::pair<std::string, std::shared_ptr<node>>(
            fmt::format("\033[34m{}\033[0m:{}:{}: Re-declaration of function {} \n", fid->file, fid->row, fid->col, fid->WORD), fid));
    }
    else // else bind
    {
        // Bind name
        n->f_table[fid->WORD][0] = header->get_child(0)->WORD;
        (*n->scope_f_table)[fid->WORD][0] = header->get_child(0)->WORD;
    }

    // n->f_id_map[fid->WORD] = header->get_child(0)->WORD;
    // syntesizhed table to return
    std::shared_ptr<ftable_type> synthesized_table = std::make_shared<ftable_type>();
    (*synthesized_table)[fid->WORD][0] = header->get_child(0)->WORD;

    // check if a FUNCTIONS node is child
    bool prog_or_funcs = (n->CLASS == "FUNCTIONS");
    bool has_functions_child = (n->num_children() > 1 && n->get_child(n->num_children() - 1)->CLASS == "FUNCTIONS");
    if (prog_or_funcs && has_functions_child)
    {
        // copy current node's f_table to child
        std::shared_ptr<node> f_child = n->get_child(n->num_children() - 1);
        // node::copy_ftable(n, c);
        node::copy_ftable(n, f_child);
        std::shared_ptr<ftable_type> child_ftable = this->preprocess_ftables(f_child, depth);
        // Copy child's f_table to current node
        node::copy_ftable(child_ftable, n);
        node::copy_ftable(child_ftable, synthesized_table);
    }
    n->pre_processed = true;
    return synthesized_table;
}
void Scope_Checker::construct_ftables(std::shared_ptr<node> n, int depth)
{
    ++depth;
    // std::cout << fmt::format("\033[34m{}\033[0mconstructing ftables for {} {}\n", std::string(depth * 2, ' '), n->WORD, n->UID);
    bool prog_or_funcs = (n->CLASS == "PROG" || n->CLASS == "FUNCTIONS");
    bool has_functions_child = (n->children_size() > 1 && n->get_child(n->children_size() - 1)->CLASS == "FUNCTIONS");
    // if no functions that affect other nodes in scope
    bool preprocessing_required = false;
    int f_index = -1;
    if (prog_or_funcs && has_functions_child)
    {
        preprocessing_required = true;
        f_index = n->children_size() - 1;
    }

    bool body_node = (n->CLASS == "BODY");
    bool has_dubfuncs_child = (n->children_size() > 2 && n->get_child(n->children_size() - 2)->CLASS == "SUBFUNCS");
    if (body_node && has_dubfuncs_child)
    {
        preprocessing_required = true;
        f_index = n->children_size() - 2;
    }
    // preprocess function descendents
    if (preprocessing_required && !n->pre_processed)
    {
        // std::cout << fmt::format("\033[34m{}\033[0mPreprocessing children of {} {}\n\n", std::string(depth * 2, ' '), n->WORD, n->UID);
        n->pre_processed = true;
        std::shared_ptr<node> f_child = n->get_child(f_index);
        // get FUNCTIONS child of SUBFUNCS if necessary
        if (f_child->CLASS == "SUBFUNCS")
        {
            f_child = f_child->get_child(0);
        }
        node::copy_ftable(n, f_child);

        std::shared_ptr<ftable_type> child_ftable = this->preprocess_ftables(f_child, depth);
        // Copy child's f_table
        node::copy_ftable(f_child, n);
        // copy to SUBFUNCS if applicable
        if (n->get_child(f_index)->UID != f_child->UID)
        {
            f_child = n->get_child(f_index);
            node::copy_ftable(n, f_child);
        }
    }
    for (auto c : n->get_children())
    {
        for (auto it = n->f_table.begin(); it != n->f_table.end(); ++it)
        {
            for (int i = 0; i < 4; ++i)
            {
                c->f_table[it->first][i] = it->second[i];
            } // popagate f_tables
        }
        this->construct_ftables(c, depth + 1);
    }
    // std::cout << fmt::format("\033[34m{}\033[0mconstructed ftables for {} {}\n", std::string(depth * 2, ' '), n->WORD, n->UID);
    // if (n->WORD == "PROG")
    //     std::cout << fmt::format("\033[34m{}\033[0mconstructed ftables for {} {}\n", std::string(depth * 2, ' '), n->WORD, n->UID);
    // return;
}
void Scope_Checker::populate_identifiers(std::shared_ptr<node> curr_node, int depth)
{
    // std::cout << fmt::format("\033[34m{}\033[0mPopulating identidiers for {} {}\n", std::string(depth * 2, ' '), n->WORD, n->UID);
    // std::cout << fmt::format("\033[34m{}\033[0mPopulating identidiers\n", std::string(depth * 2, ' '));
    if (curr_node->CLASS == "GLOBVARS")
    {
        // std::cout << fmt::format("\033[34m{}\033[0mBinding child\n", std::string(depth * 2, ' '), curr_node->WORD, curr_node->UID);
        std::shared_ptr<node> vtype = curr_node->get_child(0);
        std::shared_ptr<node> vname = curr_node->get_child(1);

        if (curr_node->scope_v_table->find(vname->get_child(0)->WORD) != curr_node->scope_v_table->end())
        {
            error.push_back(std::pair<std::string, std::pair<int, int>>(vname->get_child(0)->WORD, std::pair<int, int>(-1, -1)));
            error_messages.push_back(std::pair<std::string, std::shared_ptr<node>>(
                fmt::format("\033[34m{}\033[0m:{}:{}: Re-declaration of global-scoped variable {}\n", vname->get_child(0)->file, vname->get_child(0)->row, vname->get_child(0)->col, vname->get_child(0)->WORD), vname->get_child(0)));
        }
        else
        {
            curr_node->v_table[vname->get_child(0)->WORD] = vtype->get_child(0)->WORD;
            (*curr_node->scope_v_table)[vname->get_child(0)->WORD] = vtype->get_child(0)->WORD;
        }
    }
    // LOCVARS ==> VTYP VNAME , VTYP VNAME , VTYP VNAME ,
    //              0     1   2   3    4   5  6     7   8
    if (curr_node->CLASS == "LOCVARS")
    {
        // std::cout << fmt::format("\033[34m{}\033[0mBinding children\n", std::string(depth * 2, ' '));

        for (int i = 0; i < 7; i += 3)
        {
            std::shared_ptr<node> vtype = curr_node->get_child(i);     // 0, 3, 6
            std::shared_ptr<node> vname = curr_node->get_child(i + 1); // 1, 4, 7
            if (curr_node->scope_v_table->find(vname->get_child(0)->WORD) != curr_node->scope_v_table->end())
            {
                error.push_back(std::pair<std::string, std::pair<int, int>>(vname->get_child(0)->WORD, std::pair<int, int>(-1, -1)));
                error_messages.push_back(std::pair<std::string, std::shared_ptr<node>>(
                    fmt::format("\033[34m{}\033[0m:{}:{}: Re-declaration of local-scoped variable {}\n", vname->get_child(0)->file, vname->get_child(0)->row, vname->get_child(0)->col, vname->get_child(0)->WORD), vname->get_child(0)));
            }
            else
            {
                curr_node->v_table[vname->get_child(0)->WORD] = vtype->get_child(0)->WORD;
                (*curr_node->scope_v_table)[vname->get_child(0)->WORD] = vtype->get_child(0)->WORD;
            }
        }
    }
    // HEADER ==> FTYP FNAME ( VNAME , VNAME , VNAME ),
    //              0    1   2   3   4   5   6   7   8
    if (curr_node->CLASS == "HEADER")
    {
        // std::cout << fmt::format("\033[34m{}\033[0mBinding children\n", std::string(depth * 2, ' '));

        for (int i = 3; i < 8; i += 2)
        {
            std::shared_ptr<node> vname = curr_node->get_child(i); // 3, 5, 7
            if (curr_node->scope_v_table->find(vname->get_child(0)->WORD) != curr_node->scope_v_table->end())
            {
                error.push_back(std::pair<std::string, std::pair<int, int>>(vname->get_child(0)->WORD, std::pair<int, int>(-1, -1)));
                error_messages.push_back(std::pair<std::string, std::shared_ptr<node>>(
                    fmt::format("\033[34m{}\033[0m:{}:{}: Re-declaration of parameter variable {}\n", vname->get_child(0)->file, vname->get_child(0)->row, vname->get_child(0)->col, vname->get_child(0)->WORD), vname->get_child(0)));
            }
            else
            {
                curr_node->v_table[vname->get_child(0)->WORD] = "num"; // all parameters are numbers in BASIC
                (*curr_node->scope_v_table)[vname->get_child(0)->WORD] = "num";
            }
            // std::cout << fmt::format("\033[34m{}\033[0mBound child child {}: {}\n", std::string(depth * 2, ' '), vname->get_child(0)->WORD, curr_node->v_table[vname->get_child(0)->WORD]);
            // std::cout << fmt::format("  {}: New var {} {}\n", n->CLASS, vtype->get_child(0)->WORD, vname->get_child(0)->WORD);
        }
    }
    // std::cout << fmt::format("\033[34m{}\033[0mPopulating identidiers of children\n", std::string(depth * 2, ' '));
    for (auto c : curr_node->get_children())
    {
        node::copy_vtable(curr_node, c);
        node::copy_ftable(curr_node, c);
        this->populate_identifiers(c, depth + 1);
        if (c->CLASS == "GLOBVARS" || c->CLASS == "LOCVARS" || c->CLASS == "HEADER")
        {
            node::copy_vtable(c, curr_node);
            // std::cout << fmt::format("  {}: Copied vtable from child {}\n", n->CLASS, c->CLASS);
        }
    }
    // std::cout << fmt::format("\033[34m{}\033[0mPopulated identidiers for {} {}\n", std::string(depth * 2, ' '), curr_node->WORD, curr_node->UID);
}
bool Scope_Checker::check(std::shared_ptr<node> n, int depth)
{
    bool ret_val = true;
    // std::cout << fmt::format("\033[34m{}\033[0mChecking {} {}\n", std::string(depth * 2, ' '), n->WORD, n->UID);
    if (n->CLASS == "VID" && n->v_table.find(n->WORD) == n->v_table.end())
    {
        // std::cout << fmt::format("\033[34m{}\033[0mUndefined Variable\n", std::string(depth * 2, ' '));
        error.push_back(std::pair<std::string, std::pair<int, int>>(n->WORD, std::pair<int, int>(-1, -1)));
        error_messages.push_back(std::pair<std::string, std::shared_ptr<node>>(
            fmt::format("\033[34m{}\033[0m:{}:{}: Unknown variable {}\n", n->file, n->row, n->col, n->WORD), n));
        ret_val = false;
    }
    if (n->CLASS == "FID" && n->f_table.find(n->WORD) == n->f_table.end())
    {
        // std::cout << fmt::format("\033[34m{}\033[0mUndefined Function\n", std::string(depth * 2, ' '));
        error.push_back(std::pair<std::string, std::pair<int, int>>(n->WORD, std::pair<int, int>(-1, -1)));
        error_messages.push_back(std::pair<std::string, std::shared_ptr<node>>(
            fmt::format("\033[34m{}\033[0m:{}:{}: Unknown function {}\n", n->file, n->row, n->col, n->WORD), n));
        ret_val = false;
    }

    for (auto c : n->get_children())
    {
        // std::cout << "Checking child: " << c->WORD << " " << c->UID << "\n";
        if (!check(c, depth + 1))
        {
            ret_val = false;
        }
    }
    // std::cout << fmt::format("\033[34m{}\033[0mChecked ftables for {} {}\n", std::string(depth * 2, ' '), n->WORD, n->UID);
    return ret_val;
}

void propagate_scope_tables(std::shared_ptr<node> n)
{
    // reset lists
    if (n->CLASS == "PROG")
    {
        n->scope_f_table = std::make_shared<ftable_type>();
        n->scope_v_table = std::make_shared<vtable_type>();
    }
    // reset list of "sibling" var declarations
    if (n->CLASS == "DECL")
    {
        n->scope_v_table = std::make_shared<vtable_type>();
    }
    // reset list of "sibling" func declarations
    if (n->CLASS == "SUBFUNCS")
    {
        n->scope_f_table = std::make_shared<ftable_type>();
    }
    for (auto c : n->get_children())
    {
        c->scope_f_table = n->scope_f_table;
        c->scope_v_table = n->scope_v_table;
        propagate_scope_tables(c);
    }
}

bool Scope_Checker::run_scope_checker(int thread_number)
{
    propagate_scope_tables(root);
    this->construct_ftables(root, 0);
    this->populate_identifiers(root, 0);
    bool check_res = check(root);
    for (auto em : this->error_messages)
    {
        std::cout << em.first;
    }
    return this->error.size() == 0;
}

/*Ignore this*/
/*
void generate_tree(std::shared_ptr<node> n, pugi::xml_node productions) {
    // Find the production rule for the current node's WORD
    pugi::xpath_node lhs_xpath_node = productions.select_node((std::string("//production[@lhs='") + n->WORD + "']").c_str());

    if (!lhs_xpath_node) {
        // Terminal node (ID, KEYWORD, or LITERAL)
        n->NAME = "LEAF";
        n->UID = node_counter++;
        n->CLASS = "TERMINAL";

        // Handle terminal types by checking specific terminal classes (KEYWORD, ID, LITERAL)
        if (n->WORD == "ID") {
            n->WORD = "V_" + std::to_string(rand() % 100);  // Simulate an ID
        } else if (n->WORD == "LITERAL") {
            // Generate a literal (either numeric or string)
            int choice = rand() % 2;
            n->WORD = (choice == 0) ? std::to_string(rand() % 100) : "\"A\"";  // Numeric or string literal
        } else {
            // Use keywords from the grammar
            n->WORD = (n->WORD == "main" || n->WORD == "begin" || n->WORD == "end") ? n->WORD : "unknown_keyword";
        }

        std::cout << "Generated terminal node: " << n->WORD << " (CLASS: " << n->CLASS << ", UID: " << n->UID << ")\n";
    } else {
        // Non-terminal node
        n->NAME = "INTERNAL";
        n->CLASS = lhs_xpath_node.node().attribute("lhs").as_string();

        // Select one of the production rules (rhs)
        pugi::xml_node lhs = lhs_xpath_node.node();
        std::vector<pugi::xml_node> rhs_list;
        for (pugi::xml_node rhs = lhs.child("rhs"); rhs; rhs = rhs.next_sibling("rhs")) {
            rhs_list.push_back(rhs);
        }

        // Select a random production (rhs) to expand
        pugi::xml_node rhs = rhs_list[rand() % rhs_list.size()];
        pugi::xml_node symbol = rhs.first_child();

        std::cout << "Expanding non-terminal: " << n->WORD << " with rule: ";
        for (pugi::xml_node sym = rhs.first_child(); sym; sym = sym.next_sibling()) {
            std::cout << sym.text().as_string() << " ";
        }
        std::cout << "\n";

        // Recursively generate children
        while (symbol) {
            std::shared_ptr<node> child = std::make_shared<node>();
            child->WORD = symbol.text().as_string();
            generate_tree(child, productions);
            n->children.push_back(child);
            symbol = symbol.next_sibling();
        }
    }
}
*/