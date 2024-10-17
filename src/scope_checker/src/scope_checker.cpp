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
#include "scope_checker.hpp";
#include "automata.hpp";
#include "pugixml.hpp"
#include <cstdlib>
#include <ctime>
#include <fstream>

int node_counter = 0; // Global UID counter

Scope_Checker::Scope_Checker()
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
    // for (auto it = n->v_table.begin(); it != n->v_table.end(); ++it)
    // {
    //     std::cout << it->first << ": " << it->second << ", ";
    // }
}
void indent(int depth)
{
    for (int i = 0; i < depth; ++i)
    {
        if (i == 0)
            std::cout << "\\_";
        else if (i + 1 == depth)
            std::cout << "_/";
        else
            std::cout << "  ";
    }
}
/*
#include <fmt/core.h>
#include <fstream>

int main() {
    std::ofstream outFile("output.txt");
    if (!outFile.is_open()) {
        return -1;
    }

    std::string text = "Hello";
    int numSpaces = 5;
    std::string spaces(numSpaces, ' '); // Creates a string of 'numSpaces' spaces

    outFile << fmt::format("{}{}World\n", text, spaces);

    outFile.close();
    return 0;
}
*/
std::string print_code(std::shared_ptr<node> n, int depth, std::ofstream &code_file)
{
    auto c = n->children;
    std::string product;
    std::string indentation = std::string(depth * 4, ' ');
    std::cout << indentation << '[' << n->CLASS << "]{\n";
    // indent(depth);
    if (n->CLASS == "PROGPRIMEPRIME")
    {
        product = print_code(n->children[0], depth, code_file);
    }
    else if (n->CLASS == "PROGPRIME")
    {
        product = print_code(n->children[0], depth, code_file);
    }
    else if (n->CLASS == "PROG")
    {
        std::string main_str = print_code(n->children[0], depth, code_file);
        std::string globvars = "";
        std::string algo = "";
        std::string functions = "";
        int index = 1;

        if (n->children[1]->CLASS == "GLOBVARS")
        {
            globvars = print_code(n->children[1], depth, code_file);
            algo = print_code(n->children[2], depth, code_file);
            index = 2;
        }
        else
        {
            algo = print_code(n->children[1], depth, code_file);
        }

        if (n->children.size() == index + 2)
        {
            functions = print_code(n->children[index + 1], depth, code_file);
        }
        // product = main_str;
        int num_insertions = 1; // main
        if (globvars.length() > 0)
        {
            ++num_insertions; // globvars
            // product.append("{}\n");
            // product = fmt::format(product, globvars);
        }
        ++num_insertions; // algo
        // product.append("{}\n");
        // product = fmt::format(product, algo);
        if (functions.length() > 0)
        {
            ++num_insertions; // functions
            // product.append("{}\n");
            // product = fmt::format(product, functions);
        }
        std::cout << "PROG Num insertions: " << num_insertions << "\n";
        // if (num_insertions == 2) // only main and algo
        // {
        //     std::cout << "MAIN: \n"
        //               << main_str << "\n";
        //     std::cout << "ALGO: \n"
        //               << algo << "\n";
        //     product == fmt::format("{}{}", main_str, algo);
        // }
        // else if (num_insertions == 3)
        // {
        //     if (globvars.length() > 0)
        //     {
        //         std::cout << "MAIN: \n"
        //                   << main_str << "\n";
        //         std::cout << "GLOBVARS: \n"
        //                   << globvars << "\n";
        //         std::cout << "ALGO: \n"
        //                   << algo << "\n";
        //         product == fmt::format("{}{}{}", main_str, globvars, algo);
        //     }
        //     else
        //     {
        //         std::cout << "MAIN: \n"
        //                   << main_str << "\n";
        //         std::cout << "ALGO: \n"
        //                   << algo << "\n";
        //         std::cout << "FUNCTIONS: \n"
        //                   << functions << "\n";
        //         product == fmt::format("{}{}{}", main_str, algo, functions);
        //     }
        //     product = fmt::format
        // }
        // else
        // {
        //     std::cout << "MAIN: \n"
        //               << main_str << "\n";
        //     std::cout << "GLOBVARS: \n"
        //               << globvars << "\n";
        //     std::cout << "ALGO: \n"
        //               << algo << "\n";
        //     std::cout << "FUNCTIONS: \n"
        //               << functions << "\n";
        //     product == fmt::format("{}{}{}{}", main_str, globvars, algo, functions);
        // }
        std::string indentation = std::string(4, ' ');
        product = fmt::format("{}\n{}\n{}\n{}", main_str, globvars, algo, functions);

        std::cout << "PRODUCT: \n"
                  << product << "\n"
                  << "PROG DONE\n";
    }
    else if (n->CLASS == "INSTRUC")
    {
        product = "{}{}{}\n";
        std::string indentation = std::string(depth * 4, ' ');
        std::string command = print_code(n->children[0], depth, code_file);
        std::string semicolon = print_code(n->children[1], depth, code_file);
        std::string instruc;
        // product = fmt::format(product, indentation, command, semicolon);
        int num_insertions = 3;
        if (n->children.size() > 2)
        {
            ++num_insertions;
            // product.append("\n{}");
            instruc = print_code(n->children[2], depth, code_file);
            // product = fmt::format(product, instruc);
        }
        if (num_insertions == 3)
        {
            product = fmt::format("{}{}{}\n", indentation, command, semicolon);
        }
        else
        {

            product = fmt::format("{}{}{}\n{}", indentation, command, semicolon, instruc);
        }
    }
    else if (n->CLASS == "COMMAND")
    {
        if (n->children[0]->CLASS == "FNAME")
        {
            std::string fname = print_code(n->children[0], depth, code_file);
            std::string open = print_code(n->children[1], depth, code_file);
            std::string atomic1 = print_code(n->children[2], depth, code_file);
            std::string comma1 = print_code(n->children[3], depth, code_file);
            std::string atomic2 = print_code(n->children[4], depth, code_file);
            std::string comma2 = print_code(n->children[5], depth, code_file);
            std::string atomic3 = print_code(n->children[6], depth, code_file);
            std::string close = print_code(n->children[7], depth, code_file);
            product = "{}{}{}{}{}{}{}{}";
            product = fmt::format("{}{}{}{}{}{}{}{}", fname, open, atomic1, comma1, atomic2, comma2, atomic3, close);
        }
        else if (n->children[0]->WORD == "return" || n->children[0]->WORD == "print")
        {
            std::string keyword = print_code(n->children[0], depth, code_file);
            std::string atomic = print_code(n->children[1], depth, code_file);
            product = "{}{}";
            product = fmt::format("{}{}", keyword, atomic);
        }
        else
        {
            std::string symbol = print_code(n->children[0], depth, code_file);
            product = fmt::format("{}", symbol);
        }
    }
    else if (n->CLASS == "FUNCTIONS")
    {
        std::string decl = print_code(n->children[0], depth, code_file);
        // product = fmt::format("{}", decl);
        if (n->children.size() > 1)
        {
            // product.append("\n{}");
            std::string functions = print_code(n->children[1], depth, code_file);
            product = fmt::format("{}\n{}", decl, functions);
        }
        else
        {
            product = fmt::format("{}\n", decl);
        }
    }
    else if (n->CLASS == "SUBFUNCS")
    {
        product = print_code(n->children[0], depth + 1, code_file);
    }
    else if (n->CLASS == "DECL")
    {
        // product = "{}\n{}";
        std::string head = print_code(n->children[0], depth, code_file);
        std::string body = print_code(n->children[1], depth, code_file);
        product = fmt::format("{}\n{}", head, body);
    }
    else if (n->CLASS == "VTYP" || n->CLASS == "PROLOG" || n->CLASS == "EPILOG")
    {
        product = print_code(n->children[0], depth, code_file);
    }
    else if (n->CLASS == "VNAME")
    {
        product = print_code(n->children[0], depth, code_file);
    }
    else if (n->CLASS == "FNAME")
    {
        product = print_code(n->children[0], depth, code_file);
    }
    else if (n->CLASS == "CONST")
    {
        product = print_code(n->children[0], depth, code_file);
    }
    else if (n->CLASS == "KEYWORD" || n->CLASS == "VID" || n->CLASS == "FID" || n->CLASS == "LITERAL")
    {
        product = fmt::format("{} ", n->WORD);
        // std::cout << n->WORD <<
    }
    else if (n->CLASS == "ARG")
    {
        product = print_code(n->children[0], depth, code_file);
    }
    else if (n->CLASS == "ATOMIC")
    {
        product = print_code(n->children[0], depth, code_file);
    }
    else if (n->CLASS == "TERM")
    {
        product = print_code(n->children[0], depth, code_file);
    }
    else if (n->CLASS == "COND")
    {
        product = print_code(n->children[0], depth, code_file);
    }
    else if (n->CLASS == "GLOBVARS")
    {
        std::string vtyp = print_code(n->children[0], depth, code_file);
        std::string vname = print_code(n->children[1], depth, code_file);
        if (n->children.size() > 2)
        {
            std::string globvars = print_code(n->children[2], depth, code_file);
            product = fmt::format("{}{}{}", vtyp, vname, globvars);
        }
        else
        {
            product = fmt::format("{}{}", vtyp, vname);
        }
    }
    else if (n->CLASS == "OP")
    {
        if (c.size() > 4)
        {
            std::string binop = print_code(c[0], depth, code_file);
            std::string open = print_code(c[1], depth, code_file);
            std::string arg1 = print_code(c[2], depth, code_file);
            std::string comma = print_code(c[3], depth, code_file);
            std::string arg2 = print_code(c[4], depth, code_file);
            std::string close = print_code(c[5], depth, code_file);
            product = fmt::format("{}{}{}{}{}{}", binop, open, arg1, comma, arg2, close);
        }
        else
        {
            std::string unop = print_code(c[0], depth, code_file);
            std::string open = print_code(c[1], depth, code_file);
            std::string arg = print_code(c[2], depth, code_file);
            std::string close = print_code(c[3], depth, code_file);
            product = fmt::format("{}{}{}{}", unop, open, arg, close);
        }
    }
    else if (n->CLASS == "ASSIGN")
    {
        auto c = n->children;
        std::string vname = print_code(c[0], depth, code_file);
        if (c.size() == 2)
        {
            std::string input = print_code(c[1], depth, code_file);
            product = fmt::format("{}{}", vname, input);
        }
        else if (c.size() == 3)
        {
            std::string assignment = print_code(c[1], depth, code_file);
            std::string term = print_code(c[2], depth, code_file);
            product = fmt::format("{}{}{}", vname, assignment, term);
        }
        else // 10
        {
            std::string assignment = print_code(c[1], depth, code_file);
            std::string fname = print_code(c[2], depth, code_file);
            std::string open = print_code(c[3], depth, code_file);
            std::string atomic1 = print_code(c[4], depth, code_file);
            std::string comma1 = print_code(c[5], depth, code_file);
            std::string atomic2 = print_code(c[6], depth, code_file);
            std::string comma2 = print_code(c[7], depth, code_file);
            std::string atomic3 = print_code(c[8], depth, code_file);
            std::string close = print_code(c[9], depth, code_file);
            product = fmt::format("{}{}{}{}{}{}{}{}{}{}", vname, assignment, fname, open, atomic1, comma1, atomic2, comma2, atomic3, close);
        }
    }
    else if (n->CLASS == "COMPOSIT")
    {
        if (c.size() > 4)
        {
            std::string op = print_code(c[0], depth, code_file);
            std::string open = print_code(c[1], depth, code_file);
            std::string simple1 = print_code(c[2], depth, code_file);
            std::string comma = print_code(c[3], depth, code_file);
            std::string simple2 = print_code(c[4], depth, code_file);
            std::string close = print_code(c[5], depth, code_file);
            product = fmt::format("{}{}{}{}{}{}", op, open, simple1, comma, simple2, close);
        }
        else
        {
            std::string op = print_code(c[0], depth, code_file);
            std::string open = print_code(c[1], depth, code_file);
            std::string simple = print_code(c[2], depth, code_file);
            std::string close = print_code(c[3], depth, code_file);
            product = fmt::format("{}{}{}{}", op, open, simple, close);
        }
    }
    else if (n->CLASS == "SIMPLE")
    {
        std::string op = print_code(c[0], depth, code_file);
        std::string open = print_code(c[1], depth, code_file);
        std::string atomic1 = print_code(c[2], depth, code_file);
        std::string comma = print_code(c[3], depth, code_file);
        std::string atomic2 = print_code(c[4], depth, code_file);
        std::string close = print_code(c[5], depth, code_file);
        product = fmt::format("{}{}{}{}{}{}", op, open, atomic1, comma, atomic2, close);
    }
    else if (n->CLASS == "UNOP" || n->CLASS == "BINOP")
    {
        product = print_code(c[0], depth, code_file);
    }
    else if (n->CLASS == "LOCVARS")
    {
        std::string vtyp1 = print_code(c[0], depth, code_file);
        std::string vname1 = print_code(c[1], depth, code_file);
        std::string comma1 = print_code(c[2], depth, code_file);
        std::string vtyp2 = print_code(c[3], depth, code_file);
        std::string vname2 = print_code(c[4], depth, code_file);
        std::string comma2 = print_code(c[5], depth, code_file);
        std::string vtyp3 = print_code(c[6], depth, code_file);
        std::string vname3 = print_code(c[7], depth, code_file);
        std::string comma3 = print_code(c[8], depth, code_file);
        product = fmt::format("{}{}{}{}{}{}{}{}{}", vtyp1, vname1, comma1, vtyp2, vname2, comma2, vtyp3, vname3, comma3);
    }
    else if (n->CLASS == "HEADER")
    {
        auto c = n->children;
        std::string ftyp = print_code(c[0], depth, code_file);
        std::string fname = print_code(c[1], depth, code_file);
        std::string open = print_code(c[2], depth, code_file);
        std::string atomic1 = print_code(c[3], depth, code_file);
        std::string comma1 = print_code(c[4], depth, code_file);
        std::string atomic2 = print_code(c[5], depth, code_file);
        std::string comma2 = print_code(c[6], depth, code_file);
        std::string atomic3 = print_code(c[7], depth, code_file);
        std::string close = print_code(c[8], depth, code_file);
        product = fmt::format("{}{}{}{}{}{}{}{}{}", ftyp, fname, open, atomic1, comma1, atomic2, comma2, atomic3, close);
    }
    // else if (n->CLASS == "LITERAL")
    // {
    // }
    // else if (n->CLASS == "VID")
    // {
    // }
    // else if (n->CLASS == "FID")
    // {
    //     // indent(depth);
    //     std::cout << n->WORD << " ";
    //     code_file << n->WORD << " ";
    // }
    else if (n->CLASS == "BODY")
    {
        std::string indentation = std::string((depth) * 4, ' ');
        std::string prolog = print_code(c[0], depth, code_file);
        std::string locvars = print_code(c[1], depth + 1, code_file);
        std::string algo = print_code(c[2], depth + 1, code_file);
        std::string epilog = print_code(c[3], depth, code_file);
        if (c.size() == 5)
        {
            std::string end = print_code(c[4], depth, code_file);
            product = fmt::format("{}{}\n{}    {}\n{}\n{}{}\n{}{}", indentation, prolog, indentation, locvars, algo, indentation, epilog, indentation, end);
        }
        else
        {
            std::string subfuncs = print_code(c[4], depth, code_file);
            std::string end = print_code(c[5], depth, code_file);
            product = fmt::format("{}{}\n{}    {}\n{}\n{}{}\n{}\n{}{}", indentation, prolog, indentation, locvars, algo, indentation, epilog, subfuncs, indentation, end);
        }
    }
    else if (n->CLASS == "ALGO")
    {
        std::string begin = print_code(n->children[0], depth, code_file);
        std::string end = print_code(n->children[n->children.size() - 1], depth, code_file);
        std::string indentation = std::string(depth * 4, ' ');
        if (n->children.size() > 2)
        {
            std::string instruc = print_code(n->children[1], depth + 1, code_file);
            // std::string indentation2 = std::string("    ", depth + 1);

            product = fmt::format("{}{}\n{}{}{}", indentation, begin, instruc, indentation, end);
        }
        else
        {
            product = fmt::format("{}{}\n{}{}", indentation, begin, indentation, end);
        }
    }
    else if (n->CLASS == "BRANCH")
    {
        // indent(depth);
        // std::cout << std::string(indentation, depth);
        // code_file << std::string(indentation, depth);
        auto children = n->children;
        std::string _if = print_code(children[0], depth, code_file);  // if
        std::string cond = print_code(children[1], depth, code_file); // COND
        // std::cout << "\n";
        // code_file << "\n";
        // indent(depth);
        // std::cout << std::string(indentation, depth);
        code_file << std::string(indentation, depth);
        std::string _then = print_code(children[2], depth, code_file); // then
        // std::cout << "\n";
        // code_file << "\n";
        std::string t_algo = print_code(children[3], depth + 1, code_file); // ALGO
        // std::cout << "\n";
        // code_file << "\n";
        std::string _else = print_code(children[4], depth, code_file); // else
        // std::cout << "\n";
        // code_file << "\n";
        std::string e_algo = print_code(children[5], depth + 1, code_file); // ALGO
        std::string indentation = std::string(depth * 4, ' ');
        product = fmt::format("{}{}{}\n{}{}\n{}\n{}{}\n{}", indentation, _if, cond, indentation, _then, t_algo, indentation, _else, e_algo);
    }
    else
    {
        std::cout << "\nPrinting not defined for class " << n->CLASS << "\n";
        throw "exception";
    }
    if (product.length() == 0)
    {
        std::cout << indentation << "Product is empty\n";
    }
    else
    {
        std::cout << product << std::endl;
    }
    std::cout << std::endl
              << indentation
              << "}["
              << n->CLASS
              << "]\n";
    return product;
}
std::string node::printnode(int depth, std::string called_from)
{
    static std::unordered_map<int, bool> nodes_printed;
    std::string root_indent = std::string(depth * 2, ' ');
    std::string product;
    // pugi::xml_document doc;
    // if (!doc.load_file("CFG.xml"))
    // {
    //     std::cerr << "Error loading CFG.xml\n";
    //     return "";
    // }
    // pugi::xml_node terminal = doc.child("CFG").child("TERMINALS").child(this->CLASS.c_str());
    // std::cout << "<" << this->WORD << " uid=\"" << this->UID << "\" class=\"" << this->CLASS << "\" in_scope=\"" << is_in_scope << "\"";
    std::string head = fmt::format("{}<{} word=\"{}\" uid=\"{}\" class=\"{}\" printed=\"{}\">\n", root_indent, this->NAME, this->WORD, this->UID, this->CLASS, this->was_printed);
    std::cout << head << root_indent << this << " called from " << called_from << std::endl;
    if (nodes_printed[this->UID])
    {
        std::cout << head << fmt::format("Something went wrong, {}({}) was already printed\n", this->WORD, this->UID);
        throw "";
        return "";
    }
    nodes_printed[this->UID] = true;
    std::string inner_indent = std::string((depth + 1) * 2, ' ');
    // std::cout << "<FTABLE> ";
    std::string str_ftable = printftable(shared_from_this());
    // std::cout << "</FTABLE>\n";
    str_ftable = fmt::format("{}<FTABLE>{}</FTABLE>\n", inner_indent, str_ftable);
    // std::cout << "<VTABLE> ";
    std::string str_vtable = printvtable(shared_from_this());
    // std::cout << "</VTABLE>\n";
    str_vtable = fmt::format("{}<VTABLE>{}</VTABLE>\n", inner_indent, str_vtable);
    std::string children = "";
    if (this->children.size() > 0)
    {
        for (auto c : this->children)
        {
            if (c->was_printed)
                continue;
            children = fmt::format("{}{}", children, c->printnode(depth + 1, std::to_string(this->UID)));
        }
    }
    children = fmt::format("{}<CHILDREN>\n{}{}</CHILDREN>\n", inner_indent, children, inner_indent);
    product = fmt::format("{}{}{}{}{}</{}>\n", head, str_ftable, str_vtable, children, root_indent, this->WORD);
    // std::cout
    //     << "</" << this->WORD << ">\n";
    return product;
}

node::node() : UID(node_counter++) {}

// // Function to recursively generate an AST
// void generate_tree(std::shared_ptr<node> n, pugi::xml_node productions) {
//     pugi::xpath_node lhs_xpath_node = productions.select_node((std::string("//production[@lhs='") + n->WORD + "']").c_str());

//     if (!lhs_xpath_node) {
//         // Terminal node (ID, KEYWORD, or LITERAL)
//         n->NAME = "LEAF";
//         n->UID = node_counter++;
//         n->CLASS = "TERMINAL";

//         // Logic for terminal nodes (keywords, literals, or identifiers)
//         if (n->WORD == "ID") {
//             n->WORD = "V_" + std::to_string(rand() % 100);  // Simulate an ID
//         } else if (n->WORD == "LITERAL") {
//             n->WORD = std::to_string(rand() % 100);  // Simulate a number
//         } else {
//             // Treat as a keyword (e.g., main, begin, end)
//             n->WORD = "keyword_" + std::to_string(rand() % 5);
//         }
//     } else {
//         // Non-terminal node
//         n->NAME = "INTERNAL";
//         n->CLASS = lhs_xpath_node.node().attribute("lhs").as_string();

//         // Randomly select one production rule (rhs)
//         pugi::xml_node lhs = lhs_xpath_node.node();
//         std::vector<pugi::xml_node> rhs_list;
//         for (pugi::xml_node rhs = lhs.child("rhs"); rhs; rhs = rhs.next_sibling("rhs")) {
//             rhs_list.push_back(rhs);
//         }

//         // Select a random production (rhs) to expand
//         pugi::xml_node rhs = rhs_list[rand() % rhs_list.size()];
//         pugi::xml_node symbol = rhs.first_child();

//         // Recursively generate children
//         while (symbol) {
//             std::shared_ptr<node> child = std::make_shared<node>();
//             child->WORD = symbol.text().as_string();
//             generate_tree(child, productions);
//             n->children.push_back(child);  // Add child to current node
//             symbol = symbol.next_sibling();
//         }
//     }
// }
bool gen_automaton(std::string terminal_class, std::unordered_map<std::string, std::shared_ptr<Automaton>> &automata)
{
    std::shared_ptr<Automaton> term_automaton = std::make_shared<Automaton>();
    pugi::xml_document doc;
    if (!doc.load_file("CFG.xml"))
    {
        std::cerr << "Error loading CFG.xml\n";
        automata[terminal_class] = nullptr;
        return false;
    }
    pugi::xml_node terminal = doc.child("CFG").child("TERMINALS").child(terminal_class.c_str()).first_child();
    do
    {
        term_automaton->append_pattern(terminal.child_value(), terminal.attribute("class").value());
    } while ((terminal = terminal.next_sibling()) != pugi::xml_node());
    term_automaton->nfa_to_dfa();
    automata[terminal_class] = term_automaton;
    return true;
}

void random_pattern(std::shared_ptr<node> n)
{
    static std::unordered_map<std::string, std::shared_ptr<Automaton>> automata;
    // std::cout << "Generating pattern for: " << n->CLASS << " uid(" << n->UID << ")\n";
    static bool valid_automata = gen_automaton("VID", automata) && gen_automaton("FID", automata) && gen_automaton("LITERAL", automata);
    if (!valid_automata)
    {
        std::cerr << "\nA terminal pattern automaton was not initialised\n";
    }
    // 1% chance of incorrect pattern
    double mess_up = (((double)rand() / (double)RAND_MAX));
    // std::cout << "Probability value of messing up: " << mess_up << ": ";
    // if (mess_up < 0.1)
    // {
    //     // std::cout << "Whoops, random terminal generated for " << n->CLASS << ": ";
    //     n->WORD = n->CLASS;
    //     // n->WORD.append("_" + std::to_string(rand() % 5));
    //     // std::cout << "\" " << n->WORD << "\"\n";
    //     return;
    // }
    // else
    // {
    //     std::cout << " Didn't mess up\n";
    // }
    // randomly construct a terminal pattern based on dfa built from regexp
    if (automata.find(n->CLASS) == automata.end())
    {
        std::cout << "No automata generated for " << n->CLASS << "\n";
        throw "OOPS";
    }
    std::shared_ptr<Automaton> term_automaton = automata[n->CLASS];
    std::shared_ptr<State> dfa_state = automata[n->CLASS]->dfa_start_state;
    std::string pattern = "";
    // 20% chance of stopping at a final state, increases as pattern lengthens
    while (dfa_state != nullptr && !(dfa_state->is_final && (((double)rand() / (double)RAND_MAX) < (0.2 + (pattern.size() / 30.0)))))
    {
        // std::cout << dfa_state << " (" << dfa_state->id << ")-";
        if (dfa_state->transitions.size() == 0)
        {
            break;
        }
        std::vector<std::pair<std::string, std::vector<std::shared_ptr<State>>>> transitions;
        auto it = dfa_state->transitions.begin();
        for (; it != dfa_state->transitions.end(); ++it)
        {
            transitions.push_back(*it);
        }
        // std::cout << "Transitions populated for state\n";
        int random_transition = rand() % transitions.size();
        // std::cout << "Random transition chosen\n";
        pattern.append(transitions[random_transition].first);
        // std::cout << transitions[random_transition].first << "->\n";
        // std::cout << "Appended to pattern: " << pattern << "\n";
        dfa_state = transitions[random_transition].second[0];
    }
    // std::cout << "Final pattern: " << pattern << "\n";
    if (dfa_state == nullptr)
    {
        std::cerr << "Something went wrong,  generated pattern " << pattern << "\n";
    }
    n->WORD = pattern;
}

// TODO: Add symbol tables to generate subtrees with correctly scoped variables/functions references
int generate_tree(std::shared_ptr<node> parent, pugi::xml_node productions, int depth)
{
    // Find the production rule for the current node's WORD
    // std::cout << "Generating subtree of " << parent->CLASS << "\n";
    static int node_counter = 0;
    if (depth == 0)
    {
        node_counter = 0;
    }
    pugi::xpath_node lhs_xpath_node = productions.select_node(parent->CLASS.c_str());

    if (lhs_xpath_node != pugi::xpath_node())
    {
        // std::cout << "NONTERMINAL: " << parent->CLASS << "\n";
        // Non-terminal node
        pugi::xpath_node_set rhs_set = lhs_xpath_node.node().select_nodes("production");
        if (rhs_set.size() > 0)
        {
        }
        parent->NAME = "INTERNAL";
        parent->WORD = parent->CLASS;
        parent->UID = node_counter++;

        // Randomly select one production rule (rhs)
        pugi::xml_node lhs = lhs_xpath_node.node();
        std::vector<pugi::xml_node> rhs_list;
        for (pugi::xml_node rhs = lhs.child("production"); rhs != pugi::xml_node(); rhs = rhs.next_sibling())
        {
            rhs_list.push_back(rhs);
        }
        // Select a random production (rhs) to expand
        pugi::xml_node rhs;
        if (depth > 30 || node_counter > 200)
        {
            if (parent->WORD == "FUNCTIONS")
            {
                rhs = rhs_list[0];
                if (rhs.child("FUNCTIONS") != pugi::xml_node())
                {
                    std::cerr << "\nProduction 0 of FUNCTIONS is not FUNCTIONS ==> DECL\n";
                }
            }
            else if (parent->WORD == "INSTRUC")
            {
                rhs = rhs_list[0];
                if (rhs.child("INSTRUC") != pugi::xml_node())
                {
                    std::cerr << "\nProduction 0 of INSTRUC is not INSTRUC ==> COMMAND ;\n";
                }
            }
            else if (parent->WORD == "COMMAND")
            {
                do
                {
                    rhs = rhs_list[rand() % rhs_list.size()];
                } while (rhs.child("BRANCH") != pugi::xml_node());
            }
            else
            {
                rhs = rhs_list[rand() % rhs_list.size()];
            }
        }
        else
        {
            rhs = rhs_list[rand() % rhs_list.size()];
        }

        // Output debug information
        // std::cout << "Expanding with rule: " << lhs.name() << " ==> ";
        // for (pugi::xml_node sym = rhs.first_child(); sym != pugi::xml_node(); sym = sym.next_sibling())
        // {
        //     if (sym.name() == "KEYWORD")
        //     {
        //         std::cout << sym.child("value").child_value();
        //     }
        //     else
        //     {
        //         std::cout << sym.name();
        //     }
        //     std::cout << " ";
        // }
        // std::cout << "\n";

        pugi::xml_node symbol = rhs.first_child();
        while (symbol)
        {
            std::string sym_name = symbol.name();
            // if (sym_name == "FUNCTIONS" || sym_name == "SUBFUNCS")
            // {
            //     symbol = symbol.next_sibling();
            //     continue;
            // }
            std::shared_ptr<node> child = std::make_shared<node>();
            child->CLASS = sym_name;
            if (child->CLASS == "KEYWORD")
            {
                // std::cout << "Terminal is a KEYWORD, adding WORD: " << symbol.child("value").child_value() << "\n";
                // generate tree can't guess the correct keyword
                child->WORD = symbol.child("value").child_value();
                // 1 % chance of incorrect keyword
                bool mess_up = (((double)rand() / (double)RAND_MAX) < 0.01);
                if (mess_up)
                {
                    // std::cout << "Whoops, changed \"" << child->WORD;
                    child->WORD = "keyword_" + std::to_string(rand() % 5);
                    // std::cout << "\" to \"" << child->WORD << "\" by accident";
                }
            }
            generate_tree(child, productions, depth + 1);
            parent->children.push_back(child);
            symbol = symbol.next_sibling();
        }
    }
    else
    {
        // Terminal node (VID, FID, KEYWORD, or LITERAL)
        parent->NAME = "LEAF";
        parent->UID = node_counter++;

        // Logic for terminal nodes (keywords, literals, or identifiers)
        if (parent->CLASS == "LITERAL")
        {
            random_pattern(parent);
        }
        else if (parent->CLASS != "KEYWORD")
        {
            parent->WORD = "PLACEHOLDER";
        }
        // else is a keyword (e.g., main, begin, end), it should already set by parent of current node

        // std::cout << "Generated terminal node: " << parent->WORD << " (CLASS: " << parent->CLASS << ", UID: " << parent->UID << ", WORD: " << parent->WORD << ")\n";
    }
    return node_counter;
}

// // making a type alias because its super annoying to repeatedly define objects
// // of type std::shared_ptr<std::unordered_map<std::string, std::string>>
using ftable_type = std::unordered_map<std::string, std::string[4]>;
using vtable_type = std::unordered_map<std::string, std::string>;
// Drill down through successive chains of FUNCTIONS ==> FUNCTIONS productions
void copy_ftable(std::shared_ptr<ftable_type> f, std::shared_ptr<node> t)
{
    for (auto it = f->begin(); it != f->end(); ++it)
    {
        for (int i = 0; i < 4; ++i)
        {
            t->f_table[it->first][i] = it->second[i];
        }
    }
}
void copy_vtable(std::shared_ptr<vtable_type> f, std::shared_ptr<node> t)
{
    for (auto it = f->begin(); it != f->end(); ++it)
    {
        t->v_table[it->first] = it->second;
    }
}
void copy_ftable(std::shared_ptr<node> f, std::shared_ptr<node> t)
{
    for (auto it = f->f_table.begin(); it != f->f_table.end(); ++it)
    {
        for (int i = 0; i < 4; ++i)
        {
            t->f_table[it->first][i] = it->second[i];
        }
    }
}
void copy_vtable(std::shared_ptr<node> f, std::shared_ptr<node> t)
{
    for (auto it = f->v_table.begin(); it != f->v_table.end(); ++it)
    {
        t->v_table[it->first] = it->second;
    }
}
void copy_ftable(std::shared_ptr<ftable_type> f, std::shared_ptr<ftable_type> t)
{
    for (auto it = f->begin(); it != f->end(); ++it)
    {
        for (int i = 0; i < 4; ++i)
        {
            (*t)[it->first][i] = it->second[i];
        }
    }
}
std::shared_ptr<ftable_type> preprocess_ftables(std::shared_ptr<node> n, int depth)
{
    // for (int i = 0; i < depth; ++i)
    // {
    //     std::cout << "  ";
    // }
    // std::cout << n->UID << ": Preprocessing " << n->WORD << " uid(" << n->UID << "):\n";
    ++depth;
    // for (int i = 0; i < depth; ++i)
    // {
    //     std::cout << "  ";
    // }
    // std::cout << n->UID << ": Current ftable: ";
    // printftable(n);
    // std::cout << "\n";
    // generate a function name and types
    std::shared_ptr<node> header = n->children[0]->children[0];
    if (header->WORD != "HEADER")
    {
        std::cerr << "\nFUNCTIONS->children[0]->children[0] is not a HEADER node\n";
    }
    std::shared_ptr<node> fname = header->children[1];
    if (fname->WORD != "FNAME")
    {
        std::cerr << "\nHEADER->children[1] is supposed to be a FNAME node\n";
    }
    std::shared_ptr<node> fid = fname->children[0];
    if (fid->CLASS != "FID")
    {
        std::cerr << "\nFNAME->children[1] is supposed to be a FID node\n";
    }
    // generate name
    do
    {
        random_pattern(fid);
    } while (n->f_table.find(fid->WORD) != n->f_table.end());
    // bind type to name
    n->f_table[fid->WORD][0] = header->children[0]->WORD;
    // for (int i = 0; i < depth; ++i)
    // {
    //     std::cout << "  ";
    // }
    // std::cout << n->UID << ": Bound New Function: " << n->f_table[fid->WORD][0] << " " << fid->WORD << "\n";
    // for (int i = 0; i < depth; ++i)
    // {
    //     std::cout << "  ";
    // }
    // std::cout << n->UID << ": New ftable: ";
    // printftable(n);
    // std::cout << "\n";
    std::shared_ptr<ftable_type> synthesized_table = std::make_shared<ftable_type>();
    (*synthesized_table)[fid->WORD][0] = header->children[0]->WORD;

    // check if a FUNCTIONS node is child
    bool prog_or_funcs = (n->CLASS == "FUNCTIONS");
    bool has_functions_child = (n->children.size() > 1 && n->children[n->children.size() - 1]->CLASS == "FUNCTIONS");
    if (prog_or_funcs && has_functions_child)
    {
        // copy current node's f_table to child
        std::shared_ptr<node> f_child = n->children[n->children.size() - 1];
        // for (int i = 0; i < depth; ++i)
        // {
        //     std::cout << "  ";
        // }
        // std::cout << n->UID << ": Found FUNCTIONS node uid(" << f_child->UID << "\n";

        // std::shared_ptr<ftable_type> p = std::make_shared<ftable_type>(n->f_table);
        // std::shared_ptr<ftable_type> c = std::make_shared<ftable_type>(f_child->f_table);
        // copy_ftable(n, c);
        copy_ftable(n, f_child);
        // for (auto it = n->f_table.begin(); it != n->f_table.end(); ++it)
        // {
        //     for (int i = 0; i < 4; ++i)
        //     {
        //         f_child->f_table[it->first][i] = it->second[i];
        //         (*synthesized_table)[it->first][i] = it->second[i];
        //     }
        // }
        std::shared_ptr<ftable_type> child_ftable = preprocess_ftables(f_child, depth);
        // Copy child's f_table to current node
        copy_ftable(child_ftable, n);
        copy_ftable(child_ftable, synthesized_table);
        // for (auto it = child_ftable->begin(); it != child_ftable->end(); ++it)
        // {
        //     for (int i = 0; i < 4; ++i)
        //     {
        //         n->f_table[it->first][i] = it->second[i];
        //         (*synthesized_table)[it->first][i] = it->second[i];
        //     }
        // }
    }
    // for (int i = 0; i < depth; ++i)
    // {
    //     std::cout << "  ";
    // }
    // std::cout << n->UID << ": Final ftable: ";
    // printftable(n);
    // std::cout << "\n";
    n->pre_processed = true;
    return synthesized_table;
}
void construct_ftables(std::shared_ptr<node> n, int depth)
{
    // for (int i = 0; i < depth; ++i)
    // {
    //     std::cout << "  ";
    // }
    // std::cout << n->UID << ": " << n->WORD << " uid(" << n->UID << ") ftable population:\n";
    ++depth;
    // for (int i = 0; i < depth; ++i)
    // {
    //     std::cout << "  ";
    // }
    // std::cout << n->UID << ": Current ftable: ";
    // printftable(n);
    // std::cout << "\n";
    static pugi::xml_document doc;
    static bool file_loaded = doc.load_file("CFG.xml");
    if (!file_loaded)
    {
        // std::cout << "Error loading CFG.xml\n";
        std::cerr << "Error loading CFG.xml\n";
        return;
    }
    std::shared_ptr<ftable_type> synthesized = std::make_shared<ftable_type>();
    static pugi::xml_node productions = doc.child("CFG").child("PRODUCTIONRULES");
    static pugi::xml_node terminals = doc.child("CFG").child("TERMINALS");
    if (terminals.child(n->CLASS.c_str()) != pugi::xml_node())
    {
        return;
    }
    bool prog_or_funcs = (n->CLASS == "PROG" || n->CLASS == "FUNCTIONS");
    bool has_functions_child = (n->children.size() > 1 && n->children[n->children.size() - 1]->CLASS == "FUNCTIONS");
    // if no functions that affect other nodes in scope
    bool preprocessing_required = false;
    int f_index = -1;
    if (prog_or_funcs && has_functions_child)
    {
        preprocessing_required = true;
        f_index = n->children.size() - 1;
    }

    bool body_node = (n->CLASS == "BODY");
    bool has_dubfuncs_child = (n->children.size() > 2 && n->children[n->children.size() - 2]->CLASS == "SUBFUNCS");
    if (body_node && has_dubfuncs_child)
    {
        preprocessing_required = true;
        f_index = n->children.size() - 2;
    }
    // preprocess function descendents
    if (preprocessing_required && !n->pre_processed)
    {
        n->pre_processed = true;
        std::shared_ptr<node> f_child = n->children[f_index];
        // get FUNCTIONS child of SUBFUNCS if necessary
        if (f_child->CLASS == "SUBFUNCS")
        {
            f_child = f_child->children[0];
        }
        // for (int i = 0; i < depth; ++i)
        // {
        //     std::cout << "  ";
        // }
        // std::cout << n->UID << ": Found FUNCTIONS child uid(" << f_child << "):\n";
        // copy current node's f_table to child
        std::shared_ptr<ftable_type> p = std::make_shared<ftable_type>(n->f_table);
        std::shared_ptr<ftable_type> c = std::make_shared<ftable_type>(f_child->f_table);
        copy_ftable(p, c);
        // for (auto it = n->f_table.begin(); it != n->f_table.end(); ++it)
        // {
        //     for (int i = 0; i < 4; ++i)
        //     {
        //         f_child->f_table[it->first][i] = it->second[i];
        //     }
        // }
        std::shared_ptr<ftable_type> child_ftable = preprocess_ftables(f_child, depth);
        // Copy child's f_table
        copy_ftable(child_ftable, p);
        // for (auto it = child_ftable->begin(); it != child_ftable->end(); ++it)
        // {
        //     for (int i = 0; i < 4; ++i)
        //     {
        //         n->f_table[it->first][i] = it->second[i];
        //     }
        // }
        // copy to SUBFUNCS if applicable
        if (n->children[f_index]->UID != f_child->UID)
        {
            f_child = n->children[f_index];
            std::shared_ptr<ftable_type> c = std::make_shared<ftable_type>(f_child->f_table);
            copy_ftable(p, c);
            // for (auto it = n->f_table.begin(); it != n->f_table.end(); ++it)
            // {
            //     for (int i = 0; i < 4; ++i)
            //     {
            //         f_child->f_table[it->first][i] = it->second[i];
            //     }
            // }
        }
    }
    for (auto c : n->children)
    {
        // for (int i = 0; i < depth; ++i)
        // {
        //     std::cout << "  ";
        // }
        // std::cout << n->UID << ": CHILD " << c->WORD << " uid(" << c->UID << ")\n";
        for (auto it = n->f_table.begin(); it != n->f_table.end(); ++it)
        {
            for (int i = 0; i < 4; ++i)
            {
                c->f_table[it->first][i] = it->second[i];
            } // popagate f_tables
        }
        construct_ftables(c, depth + 1);
    }
    return;
}
void populate_identifiers(std::shared_ptr<node> n)
{
    if ((n->CLASS == "VID" || n->CLASS == "FID") && n->WORD == "PLACEHOLDER")
    {
        // 95% of selecting a legitimate ID
        double id_probability = (double)rand() / (double)RAND_MAX;
        if (id_probability > 0.05)
        {
            int rand_id = 0;
            if (n->CLASS == "VID")
            {
                rand_id = rand() % n->v_table.size();
            }
            else
            {
                rand_id = rand() % n->f_table.size();
            }
            int index = 0;
            auto vit = n->v_table.begin();
            auto fit = n->f_table.begin();
            for (; index < rand_id; ++index)
            {
                if (index == rand_id)
                {
                    if (n->CLASS == "VID")
                    {
                        n->WORD = vit->first;
                    }
                    else
                    {
                        n->WORD = fit->first;
                    }
                    return;
                }
                ++vit;
                ++fit;
            }
        }
        // will skip loop and choose random id if none are in scope
        random_pattern(n);
        if (n->CLASS == "VID")
        {
            n->is_in_scope = n->v_table.find(n->WORD) != n->v_table.end();
        }
        else if (n->CLASS == "FID")
        {
            n->is_in_scope = n->f_table.find(n->WORD) != n->f_table.end();
        }
        return;
    }
    if (n->CLASS == "GLOBVARS")
    {
        std::shared_ptr<node> vtype = n->children[0];
        std::shared_ptr<node> vname = n->children[1];
        do
        {
            random_pattern(vname->children[0]);
            // while the name is not unique
        } while (n->v_table.find(vname->children[0]->WORD) != n->v_table.end());
        n->v_table[vname->children[0]->WORD] = vtype->children[0]->WORD;
    }
    // LOCVARS ==> VTYP VNAME , VTYP VNAME , VTYP VNAME ,
    //              0     1   2   3    4   5  6     7   8
    if (n->CLASS == "LOCVARS")
    {
        for (int i = 0; i < 7; i += 3)
        {
            std::shared_ptr<node> vtype = n->children[i];     // 0, 3, 6
            std::shared_ptr<node> vname = n->children[i + 1]; // 1, 4, 7
            do
            {
                random_pattern(vname->children[0]);
                // while the name is not unique
            } while (n->v_table.find(vname->children[0]->WORD) != n->v_table.end());
            n->v_table[vname->children[0]->WORD] = vtype->children[0]->WORD;
        }
    }
    for (auto c : n->children)
    {
        std::shared_ptr<vtable_type> pt = std::make_shared<vtable_type>(n->v_table);
        std::shared_ptr<vtable_type> ct = std::make_shared<vtable_type>(c->v_table);
        copy_vtable(n, c);
        populate_identifiers(c);
        if (c->CLASS == "GLOBVARS" || c->CLASS == "LOCVARS")
        {
            std::shared_ptr<vtable_type> pt = std::make_shared<vtable_type>(n->v_table);
            std::shared_ptr<vtable_type> ct = std::make_shared<vtable_type>(c->v_table);
            copy_vtable(c, n);
        }
    }
}

void Scope_Checker::testScopeChecker()
{
    std::cout << "===== Running Random Program Test =====\n";

    pugi::xml_document doc;
    if (!doc.load_file("CFG.xml"))
    {
        std::cerr << "Error loading CFG.xml\n";
        return;
    }

    pugi::xml_node productions = doc.child("CFG").child("PRODUCTIONRULES");

    // root node for tree
    int num_nodes = 0;
    std::shared_ptr<node> root = std::make_shared<node>();
    while (num_nodes < 300)
    {
        // root->NAME = "INTERNAL";
        root->CLASS = "PROGPRIMEPRIME";
        // root->WORD = "PROGPRIMEPRIME";
        // root->UID = node_counter++;
        srand(std::time(NULL));
        num_nodes = generate_tree(root, productions, 0);
    }
    std::cout << "Populating ftables\n";
    construct_ftables(root, 0);
    populate_identifiers(root);
    std::ofstream code_file("code_file.txt");
    std::string plaintext_code = print_code(root, 0, code_file);
    std::cout << "\nPLAINTEXT:\n";
    std::cout << plaintext_code << "\n^^^^^^^^^^^^^^^^^^^^^\n";
    std::cout << num_nodes << std::endl;
    std::ofstream rand_tree("random_tree.xml");
    std::string tree = root->printnode(0, "testScopeChecker()");
    rand_tree << tree;
    rand_tree.close();
    code_file << plaintext_code;
    code_file.close();

    std::cout
        << "===== Test Complete =====\n\n";
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