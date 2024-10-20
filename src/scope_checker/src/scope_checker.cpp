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
    std::cout << "Scope Checker initialized\n";
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
    // std::cout << fmt::format("{}Preprocessing {} {}\n", std::string(depth * 2, ' '), n->WORD, n->UID);
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

    // Bind name
    n->f_table[fid->WORD][0] = header->get_child(0)->WORD;
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
    // std::cout << fmt::format("{}constructing ftables for {} {}\n", std::string(depth * 2, ' '), n->WORD, n->UID);
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
        // std::cout << fmt::format("{}Preprocessing children of {} {}\n\n", std::string(depth * 2, ' '), n->WORD, n->UID);
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
    // std::cout << fmt::format("{}constructed ftables for {} {}\n", std::string(depth * 2, ' '), n->WORD, n->UID);
    // if (n->WORD == "PROG")
    //     std::cout << fmt::format("{}constructed ftables for {} {}\n", std::string(depth * 2, ' '), n->WORD, n->UID);
    // return;
}
void Scope_Checker::populate_identifiers(std::shared_ptr<node> curr_node, int depth)
{
    // std::cout << fmt::format("{}Populating identidiers for {} {}\n", std::string(depth * 2, ' '), n->WORD, n->UID);
    // std::cout << fmt::format("{}Populating identidiers\n", std::string(depth * 2, ' '));
    if (curr_node->CLASS == "GLOBVARS")
    {
        // std::cout << fmt::format("{}Binding child\n", std::string(depth * 2, ' '), curr_node->WORD, curr_node->UID);
        std::shared_ptr<node> vtype = curr_node->get_child(0);
        std::shared_ptr<node> vname = curr_node->get_child(1);

        curr_node->v_table[vname->get_child(0)->WORD] = vtype->get_child(0)->WORD;
        // std::cout << fmt::format("{}Bound child child {}: {}\n", std::string(depth * 2, ' '), vname->get_child(0)->WORD, curr_node->v_table[vname->get_child(0)->WORD]);
        // std::cout << fmt::format("  {}: New var {} {}\n", n->CLASS, vtype->get_child(0)->WORD, vname->get_child(0)->WORD);
    }
    // LOCVARS ==> VTYP VNAME , VTYP VNAME , VTYP VNAME ,
    //              0     1   2   3    4   5  6     7   8
    if (curr_node->CLASS == "LOCVARS")
    {
        // std::cout << fmt::format("{}Binding children\n", std::string(depth * 2, ' '));

        for (int i = 0; i < 7; i += 3)
        {
            std::shared_ptr<node> vtype = curr_node->get_child(i);     // 0, 3, 6
            std::shared_ptr<node> vname = curr_node->get_child(i + 1); // 1, 4, 7
            curr_node->v_table[vname->get_child(0)->WORD] = vtype->get_child(0)->WORD;
            // std::cout << fmt::format("{}Bound child child {}: {}\n", std::string(depth * 2, ' '), vname->get_child(0)->WORD, curr_node->v_table[vname->get_child(0)->WORD]);
            // std::cout << fmt::format("  {}: New var {} {}\n", n->CLASS, vtype->get_child(0)->WORD, vname->get_child(0)->WORD);
        }
    }
    // HEADER ==> FTYP FNAME ( VNAME , VNAME , VNAME ),
    //              0    1   2   3   4   5   6   7   8
    if (curr_node->CLASS == "HEADER")
    {
        // std::cout << fmt::format("{}Binding children\n", std::string(depth * 2, ' '));

        for (int i = 3; i < 8; i += 2)
        {
            std::shared_ptr<node> vname = curr_node->get_child(i); // 3, 5, 7
            curr_node->v_table[vname->get_child(0)->WORD] = "num"; // all parameters are numbers in BASIC
            // std::cout << fmt::format("{}Bound child child {}: {}\n", std::string(depth * 2, ' '), vname->get_child(0)->WORD, curr_node->v_table[vname->get_child(0)->WORD]);
            // std::cout << fmt::format("  {}: New var {} {}\n", n->CLASS, vtype->get_child(0)->WORD, vname->get_child(0)->WORD);
        }
    }
    // std::cout << fmt::format("{}Populating identidiers of children\n", std::string(depth * 2, ' '));
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
    // std::cout << fmt::format("{}Populated identidiers for {} {}\n", std::string(depth * 2, ' '), curr_node->WORD, curr_node->UID);
}
bool Scope_Checker::check(std::shared_ptr<node> n, int depth)
{
    bool ret_val = true;
    // std::cout << fmt::format("{}Checking {} {}\n", std::string(depth * 2, ' '), n->WORD, n->UID);
    if (n->CLASS == "VID" && n->v_table.find(n->WORD) == n->v_table.end())
    {
        // std::cout << fmt::format("{}Undefined Variable\n", std::string(depth * 2, ' '));
        error.push_back(std::pair<std::string, std::pair<int, int>>(n->WORD, std::pair<int, int>(-1, -1)));
        ret_val = false;
    }
    if (n->CLASS == "FID" && n->f_table.find(n->WORD) == n->f_table.end())
    {
        // std::cout << fmt::format("{}Undefined Function\n", std::string(depth * 2, ' '));
        error.push_back(std::pair<std::string, std::pair<int, int>>(n->WORD, std::pair<int, int>(-1, -1)));
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
    // std::cout << fmt::format("{}Checked ftables for {} {}\n", std::string(depth * 2, ' '), n->WORD, n->UID);
    return ret_val;
}
bool Scope_Checker::run_scope_checker(int thread_number)
{
    std::cout << "Runing scope checker\n";
    this->construct_ftables(root, 0);
    std::cout << "Tables constructed\n";
    // std::cout << "Tables constructed\n";
    // std::cout << "Tables constructed\n";
    this->populate_identifiers(root, 0);
    std::cout << "Identifiers populated\n";
    bool check_res = check(root);
    // bool check_res = false;
    std::cout << "Scopes Checked. Result: " << (check_res ? "Success\n" : "Failure\n");
    return check_res;
}

void Scope_Checker::testScopeChecker(int i)
{
    // std::cout << "===== Running Random Program Test =====\n";

    // pugi::xml_document doc;
    // if (!doc.load_file(cfg_file.c_str()))
    // {
    //     std::cerr << "Error loading CFG.xml\n";
    //     return;
    // }

    // pugi::xml_node productions = doc.child("CFG").child("PRODUCTIONRULES");

    // // root node for tree
    // std::shared_ptr<node> root = std::make_shared<node>();
    // // std::time_t now = std::time(NULL);
    // // srand(now);
    // std::time_t now = std::time(0);
    // srand(now);
    // int num_nodes = 0;
    // for (int num_tree = 0; num_tree < 10; ++num_tree)
    // {
    //     std::shared_ptr<node> root = std::make_shared<node>();
    //     srand(rand());
    //     int num_nodes = 0;
    //     do
    //     {
    //         // root->NAME = "INTERNAL";
    //         root->clear_node();
    //         root->CLASS = "PROGPRIMEPRIME";
    //         // root->WORD = "PROGPRIMEPRIME";
    //         // root->UID = node_counter++;
    //         // std::cout << "Generating tree: " << now << "\n";
    //         num_nodes = generate_tree(root, productions, -1);
    //         node::node_id_counter = 0;
    //     } while (num_nodes < 300);
    //     std::cout << "Populating ftables\n";
    //     construct_ftables(root, 0);
    //     populate_identifiers(root);
    //     // std::cout << "PROGPRIMEPRIME num children: " << root->children_size() << std::endl;
    //     std::ofstream rand_tree(fmt::format("random_tree{}.xml", num_tree));
    //     std::string tree = root->printnode(0, "testScopeChecker()");
    //     rand_tree << tree;
    //     rand_tree.close();
    //     std::ofstream code_file(fmt::format("code_file{}.txt", num_tree));
    //     std::string plaintext_code = root->print_code(0, code_file);
    //     std::cout << "\nPLAINTEXT:\n";
    //     std::cout << plaintext_code << "\n^^^^^^^^^^^^^^^^^^^^^\n";
    //     std::cout << num_nodes << std::endl;
    //     code_file << plaintext_code;
    //     code_file.close();
    //     std::cout << "random number: " << rand() << "\n";
    //     std::cout << "num nodes: " << num_nodes << std::endl;
    // }

    // std::cout
    //     << "===== Test Complete =====\n\n";
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