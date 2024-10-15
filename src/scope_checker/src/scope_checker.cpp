// scope_checker.cpp
#include "scope_checker.hpp"
#include <cstdlib>
#include <ctime>

// Constructor
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

void Scope_Checker::testScopeChecker() {
    std::cout << "===== Running Random Program Test =====\n";
    generateRandomProgram();
    std::cout << "===== Test Complete =====\n\n";
}

int main() {
    std::srand(std::time(0));  // Seed the random generator

    Scope_Checker scopeChecker;

    // Run tests
    for (int i = 0; i < 5; ++i) {
        scopeChecker.testScopeChecker();
    }

    return 0;
}