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

// scope_checker.cpp
#include "scope_checker.hpp"
#include <cstdlib>
#include <ctime>

int node_counter = 0;  // Global UID counter

// Constructor for Scope_Checker
Scope_Checker::Scope_Checker() {
    std::cout << "Scope Checker initialized\n";
}

void Scope_Checker::pushScope() {
    scopeStack.push(SymbolTable());
}

void Scope_Checker::popScope() {
    if (!scopeStack.empty()) {
        scopeStack.pop();
    }
}

bool Scope_Checker::isVariableDeclared(const std::string &varName) {
    std::stack<SymbolTable> tempStack = scopeStack;
    while (!tempStack.empty()) {
        if (tempStack.top().variables.find(varName) != tempStack.top().variables.end()) {
            return true;
        }
        tempStack.pop();
    }
    return false;
}

bool Scope_Checker::isFunctionDeclared(const std::string &funcName) {
    std::stack<SymbolTable> tempStack = scopeStack;
    while (!tempStack.empty()) {
        if (tempStack.top().functions.find(funcName) != tempStack.top().functions.end()) {
            return true;
        }
        tempStack.pop();
    }
    return false;
}

void Scope_Checker::addVariable(const std::string &varName, const std::string &type) {
    if (!scopeStack.empty()) {
        scopeStack.top().variables[varName] = type;
    }
}

void Scope_Checker::addFunction(const std::string &funcName, const std::string &returnType) {
    if (!scopeStack.empty()) {
        scopeStack.top().functions[funcName] = returnType;
    }
}

// Random variable and function generation for testing
const std::vector<std::string> varTypes = {"num", "text"};
const std::vector<std::string> varNames = {"V_a", "V_b", "V_c", "V_x", "V_y", "V_z"};
const std::vector<std::string> funcNames = {"F_add", "F_sub", "F_mul", "F_div"};
const std::vector<std::string> operations = {"add", "sub", "mul", "div"};

void Scope_Checker::generateRandomVar() {
    std::string varType = varTypes[std::rand() % varTypes.size()];
    std::string varName = varNames[std::rand() % varNames.size()];
    std::cout << "Declaring variable " << varName << " of type " << varType << "\n";
    addVariable(varName, varType);
}

void Scope_Checker::generateRandomFunc() {
    std::string funcName = funcNames[std::rand() % funcNames.size()];
    std::string returnType = varTypes[std::rand() % varTypes.size()];
    std::cout << "Declaring function " << funcName << " with return type " << returnType << "\n";
    addFunction(funcName, returnType);
    pushScope();
    for (int i = 0; i < 3; ++i) generateRandomVar();  // Add local variables
    popScope();
}

void Scope_Checker::generateRandomOp() {
    std::string op = operations[std::rand() % operations.size()];
    std::string var1 = varNames[std::rand() % varNames.size()];
    std::string var2 = varNames[std::rand() % varNames.size()];
    std::cout << "Performing operation: " << op << "(" << var1 << ", " << var2 << ")\n";
    if (!isVariableDeclared(var1) || !isVariableDeclared(var2)) {
        std::cout << "Error: One or both variables not declared\n";
    }
}

void Scope_Checker::generateRandomProgram() {
    pushScope();  // Global scope

    // Generate global variables
    for (int i = 0; i < 2; ++i) generateRandomVar();

    // Generate functions
    for (int i = 0; i < 2; ++i) generateRandomFunc();

    // Generate operations and scope checks
    for (int i = 0; i < 3; ++i) generateRandomOp();

    popScope();  // End global scope
}

// Constructor for node
node::node() : UID(node_counter++) {}

// // Function to recursively generate an AST
// void generate_tree(std::shared_ptr<node> n, pugi::xml_node productions) {
//     // Find the production rule for the current node's WORD
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

void generate_tree(std::shared_ptr<node> n, pugi::xml_node productions) {
    // Find the production rule for the current node's WORD
    pugi::xpath_node lhs_xpath_node = productions.select_node((std::string("//production[@lhs='") + n->WORD + "']").c_str());

    if (!lhs_xpath_node) {
        // Terminal node (ID, KEYWORD, or LITERAL)
        n->NAME = "LEAF";
        n->UID = node_counter++;
        n->CLASS = "TERMINAL";

        // Logic for terminal nodes (keywords, literals, or identifiers)
        if (n->WORD == "ID") {
            n->WORD = "V_" + std::to_string(rand() % 100);  // Simulate an ID
        } else if (n->WORD == "LITERAL") {
            n->WORD = std::to_string(rand() % 100);  // Simulate a number
        } else {
            // Treat as a keyword (e.g., main, begin, end)
            n->WORD = "keyword_" + std::to_string(rand() % 5);
        }

       // std::cout << "Generated terminal node: " << n->WORD << " (CLASS: " << n->CLASS << ", UID: " << n->UID << ")\n";
    } else {
        // Non-terminal node
        n->NAME = "INTERNAL";
        n->CLASS = lhs_xpath_node.node().attribute("lhs").as_string();

        // Randomly select one production rule (rhs)
        pugi::xml_node lhs = lhs_xpath_node.node();
        std::vector<pugi::xml_node> rhs_list;
        for (pugi::xml_node rhs = lhs.child("rhs"); rhs; rhs = rhs.next_sibling("rhs")) {
            rhs_list.push_back(rhs);
        }

        // Select a random production (rhs) to expand
        pugi::xml_node rhs = rhs_list[rand() % rhs_list.size()];
        pugi::xml_node symbol = rhs.first_child();

        // Output debug information
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


void Scope_Checker::testScopeChecker() {
    std::cout << "===== Running Random Program Test =====\n";
    
    pugi::xml_document doc;
    if (!doc.load_file("CFG.xml")) {
        std::cerr << "Error loading CFG.xml\n";
        return;
    }

    pugi::xml_node productions = doc.child("PRODUCTIONRULES");

    // Create the root node for the AST
    std::shared_ptr<node> root = std::make_shared<node>();
    root->NAME = "INTERNAL";
    root->CLASS = "PROGPRIMEPRIME";
    root->WORD = "PROGPRIMEPRIME";
    root->UID = node_counter++;

    generate_tree(root, productions);

    std::cout << "===== Test Complete =====\n\n";
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