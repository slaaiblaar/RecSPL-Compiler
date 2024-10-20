// #include "code_generator.hpp"
// #include <fmt/core.h>

// Code_Generator::Code_Generator()
// {
//     std::cout << "Code Generator\n";
// }


#include "code_generator.hpp"
#include <fmt/core.h>

Code_Generator::Code_Generator() {
    std::cout << "Code Generator Initialized\n";
}

// Generates intermediate code for the AST starting from the root node
std::string Code_Generator::generate(std::shared_ptr<node> root) {
    // Start generating code from the root node, assuming it's a program or function
    return TransStat(root, vtable, ftable);
}

// Translates an expression node to intermediate code
std::string Code_Generator::TransExp(std::shared_ptr<node> Exp, vtable_type& vtable, ftable_type& ftable, std::string& place) {
    if (Exp->CLASS == "num") {
        std::string v = Exp->WORD; // Assuming num literal is in WORD
        return fmt::format("{} := {}\n", place, v);
    }
    if (Exp->CLASS == "id") {
        std::string x = vtable[Exp->WORD];
        return fmt::format("{} := {}\n", place, x);
    }
    if (Exp->CLASS == "unop") {
        std::string place1 = newVar();
        std::string code1 = TransExp(Exp->get_child(0), vtable, ftable, place1);
        std::string op = Exp->WORD; // Unary operation type in WORD
        return code1 + fmt::format("{} := {} {}\n", place, op, place1);
    }
    if (Exp->CLASS == "binop") {
        std::string place1 = newVar();
        std::string place2 = newVar();
        std::string code1 = TransExp(Exp->get_child(0), vtable, ftable, place1);
        std::string code2 = TransExp(Exp->get_child(1), vtable, ftable, place2);
        std::string op = Exp->WORD; // Binary operation type in WORD
        return code1 + code2 + fmt::format("{} := {} {} {}\n", place, place1, op, place2);
    }
    // // Handle function calls
    // if (Exp->CLASS == "call") {
    //     std::vector<std::string> args;
    //     std::string code1 = TransExps(Exp->get_children(), vtable, ftable, args);
    //     std::string fname = ftable[Exp->WORD][0];
    //     return code1 + fmt::format("{} := CALL {}({})\n", place, fname, fmt::join(args, ", "));
        
    // }

    // Handle function calls
if (Exp->CLASS == "call") {
    std::vector<std::string> args;
    std::string code1 = TransExps(Exp->get_children(), vtable, ftable, args);
    std::string fname = ftable[Exp->WORD][0];
    
    // Manually join the arguments using std::ostringstream
    std::ostringstream argsStream;
    for (size_t i = 0; i < args.size(); ++i) {
        argsStream << args[i];
        if (i != args.size() - 1) {
            argsStream << ", ";  // Add comma between arguments
        }
    }

    return code1 + fmt::format("{} := CALL {}({})\n", place, fname, argsStream.str());
}
    return "";
}

// Translates a statement node to intermediate code
std::string Code_Generator::TransStat(std::shared_ptr<node> Stat, vtable_type& vtable, ftable_type& ftable) {
    if (Stat->CLASS == "assign") {
        std::string place = newVar();
        std::string code1 = TransExp(Stat->get_child(1), vtable, ftable, place);
        std::string x = vtable[Stat->get_child(0)->WORD];
        return code1 + fmt::format("{} := {}\n", x, place);
    }
    if (Stat->CLASS == "if") {
        std::string label1 = newLabel();
        std::string label2 = newLabel();
        std::string code1 = TransCond(Stat->get_child(0), label1, label2, vtable, ftable);
        std::string code2 = TransStat(Stat->get_child(1), vtable, ftable);
        return code1 + fmt::format("LABEL {}\n", label1) + code2 + fmt::format("LABEL {}\n", label2);
    }
    if (Stat->CLASS == "return") {
        std::string place = newVar();
        std::string code1 = TransExp(Stat->get_child(0), vtable, ftable, place);
        return code1 + fmt::format("RETURN {}\n", place);
    }
    // Additional statement translations (e.g., while, repeat, function declarations) can be added here
    return "";
}

// Translates a condition node to intermediate code
std::string Code_Generator::TransCond(std::shared_ptr<node> Cond, const std::string& labelTrue, const std::string& labelFalse, vtable_type& vtable, ftable_type& ftable) {
    std::string t1 = newVar();
    std::string t2 = newVar();
    std::string code1 = TransExp(Cond->get_child(0), vtable, ftable, t1);
    std::string code2 = TransExp(Cond->get_child(1), vtable, ftable, t2);
    std::string op = Cond->WORD; // Relational operator in WORD
    return code1 + code2 + fmt::format("IF {} {} {} THEN {} ELSE {}\n", t1, op, t2, labelTrue, labelFalse);
}

// Translates multiple expressions for function arguments
std::string Code_Generator::TransExps(std::vector<std::shared_ptr<node>> Exps, vtable_type& vtable, ftable_type& ftable, std::vector<std::string>& args) {
    std::string result;
    for (auto& Exp : Exps) {
        std::string place = newVar();
        result += TransExp(Exp, vtable, ftable, place);
        args.push_back(place);
    }
    return result;
}

// Generates a new temporary variable
std::string Code_Generator::newVar() {
    static int varCount = 0;
    return fmt::format("t{}", varCount++);
}

// Generates a new label for conditional jumps
std::string Code_Generator::newLabel() {
    static int labelCount = 0;
    return fmt::format("L{}", labelCount++);
}
