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

Scope_Checker::Scope_Checker(std::string cfg_file) : cfg_file(cfg_file)
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

std::string printftable(std::shared_ptr<node> n)
{
    std::string ftable = "";
    for (auto it = n->f_table.begin(); it != n->f_table.end(); ++it)
    {
        ftable = fmt::format("{}{}() ==> {}, ", ftable, it->first, it->second[0]);
    }
    return ftable;
}
std::string printvtable(std::shared_ptr<node> n)
{
    std::string vtable = "";
    for (auto it = n->v_table.begin(); it != n->v_table.end(); ++it)
    {
        vtable = fmt::format("{}{}: {}, ", vtable, it->first, it->second);
    }
    return vtable;
}

// bool gen_automaton(std::string terminal_class, std::unordered_map<std::string, std::shared_ptr<Automaton>> &automata)
// {
//     std::shared_ptr<Automaton> term_automaton = std::make_shared<Automaton>();
//     pugi::xml_document doc;
//     if (!doc.load_file(cfg_file.c_str()))
//     {
//         std::cerr << "Error loading CFG.xml\n";
//         automata[terminal_class] = nullptr;
//         return false;
//     }
//     pugi::xml_node terminal = doc.child("CFG").child("TERMINALS").child(terminal_class.c_str()).first_child();
//     do
//     {
//         term_automaton->append_pattern(terminal.child_value(), terminal.attribute("class").value());
//     } while ((terminal = terminal.next_sibling()) != pugi::xml_node());
//     term_automaton->nfa_to_dfa();
//     automata[terminal_class] = term_automaton;
//     return true;
// }

// void random_pattern(std::shared_ptr<node> n)
// {
//     static std::unordered_map<std::string, std::shared_ptr<Automaton>> automata;
//     // std::cout << "Generating pattern for: " << n->CLASS << " uid(" << n->UID << ")\n";
//     static bool valid_automata = gen_automaton("VID", automata) && gen_automaton("FID", automata) && gen_automaton("LITERAL", automata);
//     if (!valid_automata)
//     {
//         std::cerr << "\nA terminal pattern automaton was not initialised\n";
//     }
//     // 1% chance of incorrect pattern
//     double mess_up = (((double)rand() / (double)RAND_MAX));
//     // randomly construct a terminal pattern based on dfa built from regexp
//     if (automata.find(n->CLASS) == automata.end())
//     {
//         std::cerr << "\nAn error occured while generating an automata for the terminal class " << n->CLASS << "\n";
//         throw;
//     }
//     std::shared_ptr<Automaton> term_automaton = automata[n->CLASS];
//     std::shared_ptr<State> dfa_state = automata[n->CLASS]->dfa_start_state;
//     std::string pattern = "";
//     // 20% chance of stopping at a final state, increases as pattern lengthens
//     while (dfa_state != nullptr && !(dfa_state->is_final && (((double)rand() / (double)RAND_MAX) < (0.2 + (pattern.size() / 30.0)))))
//     {
//         if (dfa_state->transitions.size() == 0)
//         {
//             break;
//         }
//         std::vector<std::pair<std::string, std::vector<std::shared_ptr<State>>>> transitions;
//         auto it = dfa_state->transitions.begin();
//         for (; it != dfa_state->transitions.end(); ++it)
//         {
//             transitions.push_back(*it);
//         }
//         int random_transition = rand() % transitions.size();
//         pattern.append(transitions[random_transition].first);
//         dfa_state = transitions[random_transition].second[0];
//     }
//     if (dfa_state == nullptr)
//     {
//         std::cerr << "Something went wrong,  generated pattern " << pattern << "\n";
//     }
//     n->WORD = pattern;
// }

// // TODO: Add symbol tables to generate subtrees with correctly scoped variables/functions references
// int generate_tree(std::shared_ptr<node> parent, pugi::xml_node productions, int depth)
// {
//     if (parent->subtree_generated)
//     {
//         return node::node_id_counter;
//     }
//     parent->subtree_generated = true;
//     // Find the production rule for the current node's WORD
//     static int node_counter = 0;
//     if (depth == -1)
//     {
//         node_counter = 0;
//         depth = 0;
//         node::node_id_counter = 1;
//     }
//     pugi::xpath_node lhs_xpath_node = productions.select_node(parent->CLASS.c_str());

//     if (lhs_xpath_node != pugi::xpath_node())
//     {
//         // Non-terminal node
//         pugi::xpath_node_set rhs_set = lhs_xpath_node.node().select_nodes("production");
//         if (rhs_set.size() > 0)
//         {
//         }
//         parent->NAME = "INTERNAL";
//         parent->WORD = parent->CLASS;
//         // Randomly select one production rule (rhs)
//         pugi::xml_node lhs = lhs_xpath_node.node();
//         std::vector<pugi::xml_node> rhs_list;
//         for (pugi::xml_node rhs = lhs.child("production"); rhs != pugi::xml_node(); rhs = rhs.next_sibling())
//         {
//             rhs_list.push_back(rhs);
//         }
//         // Select a random production (rhs) to expand
//         pugi::xml_node rhs;
//         if (depth > 60 || node_counter > 1000)
//         {
//             if (parent->WORD == "FUNCTIONS")
//             {
//                 rhs = rhs_list[0];
//                 if (rhs.child("FUNCTIONS") != pugi::xml_node())
//                 {
//                     std::cerr << "\nProduction 0 of FUNCTIONS is not FUNCTIONS ==> DECL\n";
//                 }
//             }
//             else if (parent->WORD == "INSTRUC")
//             {
//                 rhs = rhs_list[0];
//                 if (rhs.child("INSTRUC") != pugi::xml_node())
//                 {
//                     std::cerr << "\nProduction 0 of INSTRUC is not INSTRUC ==> COMMAND ;\n";
//                 }
//             }
//             else if (parent->WORD == "COMMAND")
//             {
//                 do
//                 {
//                     rhs = rhs_list[rand() % rhs_list.size()];
//                 } while (rhs.child("BRANCH") != pugi::xml_node());
//             }
//             else
//             {
//                 rhs = rhs_list[rand() % rhs_list.size()];
//             }
//         }
//         else
//         {
//             rhs = rhs_list[rand() % rhs_list.size()];
//         }

//         pugi::xml_node symbol = rhs.first_child();
//         int child_index = 0;
//         while (symbol != pugi::xml_node())
//         {
//             std::string sym_name = symbol.name();
//             if (child_index != parent->children_size())
//             {
//                 if (sym_name == "KEYWORD")
//                 {
//                     sym_name = symbol.child("value").child_value();
//                 }
//                 std::cerr << fmt::format("\nchild_index={}, num children={}, symbol={}\n", child_index, parent->children_size(), sym_name);
//                 throw;
//             }

//             std::shared_ptr<node> child = std::make_shared<node>();
//             child->CLASS = sym_name;
//             if (child->CLASS == "KEYWORD")
//             {
//                 // generate tree can't guess the correct keyword
//                 child->WORD = symbol.child("value").child_value();
//                 // 0.1 % chance of incorrect keyword
//                 bool mess_up = (((double)rand() / (double)RAND_MAX) < 0.001);
//                 // if (mess_up)
//                 // {
//                 //     child->WORD = fmt::format("keyword_{}({})", std::to_string(rand() % 5), child->WORD);
//                 // }
//             }
//             generate_tree(child, productions, depth + 1);

//             bool is_new = parent->add_child(child, child_index);
//             if (!is_new)
//             {
//                 std::cerr << fmt::format("\n{}{} {} \"{}\" ATTEMPTED DUPLICATE CHILD OF {}\n", std::string(depth * 2, ' '), child->CLASS, child->UID, child->WORD, parent->WORD);
//                 throw;
//             }
//             else
//             {
//                 ++child_index;
//             }

//             symbol = symbol.next_sibling();
//         }
//     }
//     else
//     {
//         // Terminal node (VID, FID, KEYWORD, or LITERAL)
//         parent->NAME = "LEAF";

//         // Logic for terminal nodes (keywords, literals, or identifiers)
//         if (parent->CLASS == "LITERAL")
//         {
//             random_pattern(parent);
//         }
//         else if (parent->CLASS != "KEYWORD")
//         {
//             parent->WORD = "PLACEHOLDER";
//         }
//     }

//     return node::node_id_counter;
// }

// // making a type alias because its super annoying to repeatedly define objects
// // of type std::shared_ptr<std::unordered_map<std::string, std::string>>
using ftable_type = std::unordered_map<std::string, std::string[4]>;
using vtable_type = std::unordered_map<std::string, std::string>;
// Drill down through successive chains of FUNCTIONS ==> FUNCTIONS productions
// void copy_ftable(std::shared_ptr<ftable_type> f, std::shared_ptr<node> t)
// {
//     for (auto it = f->begin(); it != f->end(); ++it)
//     {
//         for (int i = 0; i < 4; ++i)
//         {
//             t->f_table[it->first][i] = it->second[i];
//         }
//     }
// }
// void copy_vtable(std::shared_ptr<vtable_type> f, std::shared_ptr<node> t)
// {
//     for (auto it = f->begin(); it != f->end(); ++it)
//     {
//         t->v_table[it->first] = it->second;
//     }
// }
// void copy_ftable(std::shared_ptr<node> f, std::shared_ptr<node> t)
// {
//     for (auto it = f->f_table.begin(); it != f->f_table.end(); ++it)
//     {
//         for (int i = 0; i < 4; ++i)
//         {
//             t->f_table[it->first][i] = it->second[i];
//         }
//     }
// }
// void copy_vtable(std::shared_ptr<node> f, std::shared_ptr<node> t)
// {
//     for (auto it = f->v_table.begin(); it != f->v_table.end(); ++it)
//     {
//         t->v_table[it->first] = it->second;
//     }
// }
// void copy_ftable(std::shared_ptr<ftable_type> f, std::shared_ptr<ftable_type> t)
// {
//     for (auto it = f->begin(); it != f->end(); ++it)
//     {
//         for (int i = 0; i < 4; ++i)
//         {
//             (*t)[it->first][i] = it->second[i];
//         }
//     }
// }
// std::shared_ptr<ftable_type> preprocess_ftables(std::shared_ptr<node> n, int depth)
// {
//     ++depth;
//     std::shared_ptr<node> header = n->get_child(0)->get_child(0);
//     if (header->WORD != "HEADER")
//     {
//         std::cerr << "\nFUNCTIONS->children[0]->children[0] is not a HEADER node\n";
//     }
//     std::shared_ptr<node> fname = header->get_child(1);
//     if (fname->WORD != "FNAME")
//     {
//         std::cerr << "\nHEADER->children[1] is supposed to be a FNAME node\n";
//     }
//     std::shared_ptr<node> fid = fname->get_child(0);
//     if (fid->CLASS != "FID")
//     {
//         std::cerr << "\nFNAME->children[1] is supposed to be a FID node\n";
//     }
//     // generate name
//     do
//     {
//         random_pattern(fid);
//     } while (n->f_table.find(fid->WORD) != n->f_table.end());
//     // bind type to name
//     n->f_table[fid->WORD][0] = header->get_child(0)->WORD;
//     std::shared_ptr<ftable_type> synthesized_table = std::make_shared<ftable_type>();
//     (*synthesized_table)[fid->WORD][0] = header->get_child(0)->WORD;

//     // check if a FUNCTIONS node is child
//     bool prog_or_funcs = (n->CLASS == "FUNCTIONS");
//     bool has_functions_child = (n->num_children() > 1 && n->get_child(n->num_children() - 1)->CLASS == "FUNCTIONS");
//     if (prog_or_funcs && has_functions_child)
//     {
//         // copy current node's f_table to child
//         std::shared_ptr<node> f_child = n->get_child(n->num_children() - 1);
//         // copy_ftable(n, c);
//         copy_ftable(n, f_child);
//         std::shared_ptr<ftable_type> child_ftable = preprocess_ftables(f_child, depth);
//         // Copy child's f_table to current node
//         copy_ftable(child_ftable, n);
//         copy_ftable(child_ftable, synthesized_table);
//     }
//     n->pre_processed = true;
//     return synthesized_table;
// }
// void construct_ftables(std::shared_ptr<node> n, int depth)
// {
//     ++depth;
//     static pugi::xml_document doc;
//     static bool file_loaded = doc.load_file(cfg_file.c_str());
//     if (!file_loaded)
//     {
//         // std::cout << "Error loading CFG.xml\n";
//         std::cerr << "Error loading CFG.xml\n";
//         return;
//     }
//     std::shared_ptr<ftable_type> synthesized = std::make_shared<ftable_type>();
//     static pugi::xml_node productions = doc.child("CFG").child("PRODUCTIONRULES");
//     static pugi::xml_node terminals = doc.child("CFG").child("TERMINALS");
//     if (terminals.child(n->CLASS.c_str()) != pugi::xml_node())
//     {
//         return;
//     }
//     bool prog_or_funcs = (n->CLASS == "PROG" || n->CLASS == "FUNCTIONS");
//     bool has_functions_child = (n->children_size() > 1 && n->get_child(n->children_size() - 1)->CLASS == "FUNCTIONS");
//     // if no functions that affect other nodes in scope
//     bool preprocessing_required = false;
//     int f_index = -1;
//     if (prog_or_funcs && has_functions_child)
//     {
//         preprocessing_required = true;
//         f_index = n->children_size() - 1;
//     }

//     bool body_node = (n->CLASS == "BODY");
//     bool has_dubfuncs_child = (n->children_size() > 2 && n->get_child(n->children_size() - 2)->CLASS == "SUBFUNCS");
//     if (body_node && has_dubfuncs_child)
//     {
//         preprocessing_required = true;
//         f_index = n->children_size() - 2;
//     }
//     // preprocess function descendents
//     if (preprocessing_required && !n->pre_processed)
//     {
//         n->pre_processed = true;
//         std::shared_ptr<node> f_child = n->get_child(f_index);
//         // get FUNCTIONS child of SUBFUNCS if necessary
//         if (f_child->CLASS == "SUBFUNCS")
//         {
//             f_child = f_child->get_child(0);
//         }
//         std::shared_ptr<ftable_type> p = std::make_shared<ftable_type>(n->f_table);
//         std::shared_ptr<ftable_type> c = std::make_shared<ftable_type>(f_child->f_table);
//         copy_ftable(p, c);

//         std::shared_ptr<ftable_type> child_ftable = preprocess_ftables(f_child, depth);
//         // Copy child's f_table
//         copy_ftable(child_ftable, p);
//         // copy to SUBFUNCS if applicable
//         if (n->get_child(f_index)->UID != f_child->UID)
//         {
//             f_child = n->get_child(f_index);
//             std::shared_ptr<ftable_type> c = std::make_shared<ftable_type>(f_child->f_table);
//             copy_ftable(p, c);
//         }
//     }
//     for (auto c : n->get_children())
//     {
//         for (auto it = n->f_table.begin(); it != n->f_table.end(); ++it)
//         {
//             for (int i = 0; i < 4; ++i)
//             {
//                 c->f_table[it->first][i] = it->second[i];
//             } // popagate f_tables
//         }
//         construct_ftables(c, depth + 1);
//     }
//     return;
// }
// void populate_identifiers(std::shared_ptr<node> n)
// {
//     if ((n->CLASS == "VID" || n->CLASS == "FID") && n->WORD == "PLACEHOLDER")
//     {
//         // 95% of selecting a legitimate ID
//         double id_probability = (double)rand() / (double)RAND_MAX;
//         if (id_probability > 0.05)
//         {
//             int rand_id = 0;
//             if (n->CLASS == "VID")
//             {
//                 rand_id = rand() % n->v_table.size();
//             }
//             else
//             {
//                 rand_id = rand() % n->f_table.size();
//             }
//             int index = 0;
//             auto vit = n->v_table.begin();
//             auto fit = n->f_table.begin();
//             for (; index < rand_id; ++index)
//             {
//                 if (index == rand_id)
//                 {
//                     if (n->CLASS == "VID")
//                     {
//                         n->WORD = vit->first;
//                     }
//                     else
//                     {
//                         n->WORD = fit->first;
//                     }
//                     return;
//                 }
//                 ++vit;
//                 ++fit;
//             }
//         }
//         // will skip loop and choose random id if none are in scope
//         random_pattern(n);
//         if (n->CLASS == "VID")
//         {
//             n->is_in_scope = n->v_table.find(n->WORD) != n->v_table.end();
//         }
//         else if (n->CLASS == "FID")
//         {
//             n->is_in_scope = n->f_table.find(n->WORD) != n->f_table.end();
//         }
//         return;
//     }
//     if (n->CLASS == "GLOBVARS")
//     {
//         std::shared_ptr<node> vtype = n->get_child(0);
//         std::shared_ptr<node> vname = n->get_child(1);
//         do
//         {
//             random_pattern(vname->get_child(0));
//             // while the name is not unique
//         } while (n->v_table.find(vname->get_child(0)->WORD) != n->v_table.end());
//         n->v_table[vname->get_child(0)->WORD] = vtype->get_child(0)->WORD;
//     }
//     // LOCVARS ==> VTYP VNAME , VTYP VNAME , VTYP VNAME ,
//     //              0     1   2   3    4   5  6     7   8
//     if (n->CLASS == "LOCVARS")
//     {
//         for (int i = 0; i < 7; i += 3)
//         {
//             std::shared_ptr<node> vtype = n->get_child(i);     // 0, 3, 6
//             std::shared_ptr<node> vname = n->get_child(i + 1); // 1, 4, 7
//             do
//             {
//                 random_pattern(vname->get_child(0));
//                 // while the name is not unique
//             } while (n->v_table.find(vname->get_child(0)->WORD) != n->v_table.end());
//             n->v_table[vname->get_child(0)->WORD] = vtype->get_child(0)->WORD;
//         }
//     }
//     for (auto c : n->get_children())
//     {
//         std::shared_ptr<vtable_type> pt = std::make_shared<vtable_type>(n->v_table);
//         std::shared_ptr<vtable_type> ct = std::make_shared<vtable_type>(c->v_table);
//         copy_vtable(n, c);
//         populate_identifiers(c);
//         if (c->CLASS == "GLOBVARS" || c->CLASS == "LOCVARS")
//         {
//             std::shared_ptr<vtable_type> pt = std::make_shared<vtable_type>(n->v_table);
//             std::shared_ptr<vtable_type> ct = std::make_shared<vtable_type>(c->v_table);
//             copy_vtable(c, n);
//         }
//     }
// }

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