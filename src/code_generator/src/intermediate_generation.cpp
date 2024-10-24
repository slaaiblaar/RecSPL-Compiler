#include "code_generator.hpp"
#include <fmt/core.h>
#include <sstream>
#include <iostream>

// Phase A: Generates intermediate code for the AST starting from the root node
std::string Code_Generator::generate(std::shared_ptr<node> root) {
    std::string code = TransStat(root, vtable, ftable);
    std::cout << "Intermediate Code: " << code << std::endl;
    return code;
}
std::string Code_Generator::trans(std::shared_ptr<node> root) {
    
    // if (root->CLASS == "")
    //     return prog(root);
    // if (root->CLASS == "GLOBVARS")
    //     return globvars(root);
    // if (root->CLASS == "VTYP")
    //     return vtyp(root);
    // if (root->CLASS == "VNAME")
    //     return vname(root);
    // if (root->CLASS == "ALGO")
    //     return algo(root);
    // if (root->CLASS == "INSTRUC")
    //     return instruc(root);
    // if (root->CLASS == "COMMAND")
    //     return command(root);
    // if (root->CLASS == "ATOMIC")
    //     return atomic(root);
    // if (root->CLASS == "CONST")
    //     return cons(root);
    // if (root->CLASS == "ASSIGN")
    //     return assign(root);
    // if (root->CLASS == "BRANCH")
    //     return branch(root);
    // if (root->CLASS == "TERM")
    //     return term(root);
    // if (root->CLASS == "OP")
    //     return op(root);
    // if (root->CLASS == "ARG")
    //     return arg(root);
    // if (root->CLASS == "COND")
    //     return cond(root);
    // if (root->CLASS == "SIMPLE")
    //     return simple(root);
    // if (root->CLASS == "COMPOSIT")
    //     return composit(root);
    // if (root->CLASS == "UNOP")
    //     return unop(root);
    // if (root->CLASS == "BINOP")
    //     return binop(root);
    // if (root->CLASS == "FNAME")
    //     return fname(root);
    // if (root->CLASS == "FUNCTIONS")
    //     return functions(root);
    // if (root->CLASS == "DECL")
    //     return decl(root);
    // if (root->CLASS == "HEADER")
    //     return header(root);
    // if (root->CLASS == "BODY")
    //     return body(root);
    // if (root->CLASS == "PROLOG")
    //     return prolog(root);
    // if (root->CLASS == "EPILOG")
    //     return epilog(root);
    // if (root->CLASS == "LOCVARS")
    //     return locvars(root);
    // if (root->CLASS == "SUBFUNCS")
    //     return subfuncs(root);
    // if (root->CLASS == "KEYWORD")
    //     return keyword(root);
    // if (root->CLASS == "LITERAL")
    //     return keyword(root);

}

// Phase A: Translates an expression node to intermediate code
std::string Code_Generator::TransExp(std::shared_ptr<node> Exp, sym_table_type& vtable, sym_table_type& ftable, std::string& place) {
    if (Exp->CLASS == "LITERAL") {
        std::string v = Exp->WORD;
        return fmt::format("{} := {}\n", place, v);
    }
    if (Exp->CLASS == "VID") {
        std::string x = Exp->WORD;
        return fmt::format("{} := {}\n", place, x);
    }
    if (Exp->CLASS == "BINOP") {
        std::string place1 = newVar();
        std::string place2 = newVar();
        std::string code1 = TransExp(Exp->get_child(0), vtable, ftable, place1);
        std::string code2 = TransExp(Exp->get_child(1), vtable, ftable, place2);
        std::string op = Exp->WORD;
        return code1 + code2 + fmt::format("{} := {} {} {}\n", place, place1, op, place2);
    }
    return "";
}

// Phase A: Translates a statement node to intermediate code
std::string Code_Generator::TransStat(std::shared_ptr<node> Stat, sym_table_type& vtable, sym_table_type& ftable) {
    if (Stat->CLASS == "ASSIGN") {
        std::string place = newVar();
        std::string code1 = TransExp(Stat->get_child(1), vtable, ftable, place);
        std::string x = Stat->get_child(0)->WORD;
        return code1 + fmt::format("{} := {}\n", x, place);
    }
    if (Stat->CLASS == "BRANCH") {
        std::string label1 = newLabel();
        std::string label2 = newLabel();
        std::string code1 = TransCond(Stat->get_child(0), label1, label2, vtable, ftable);
        std::string code2 = TransStat(Stat->get_child(1), vtable, ftable);
        return code1 + fmt::format("LABEL {}\n", label1) + code2 + fmt::format("LABEL {}\n", label2);
    }
    return "";
}

// Translates a condition node to intermediate code (Phase A)
std::string Code_Generator::TransCond(std::shared_ptr<node> Cond, const std::string& labelTrue, const std::string& labelFalse, sym_table_type& vtable, sym_table_type& ftable) {
    std::string t1 = newVar();
    std::string t2 = newVar();
    std::string code1 = TransExp(Cond->get_child(0), vtable, ftable, t1);
    std::string code2 = TransExp(Cond->get_child(1), vtable, ftable, t2);
    std::string op = Cond->WORD;
    return code1 + code2 + fmt::format("IF {} {} {} THEN GOTO {} ELSE GOTO {}\n", t1, op, t2, labelTrue, labelFalse);
}

// Translates multiple expressions for function arguments (Phase A)
std::string Code_Generator::TransExps(std::vector<std::shared_ptr<node>> Exps, sym_table_type& vtable, sym_table_type& ftable, std::vector<std::string>& args) {
    std::string result;
    for (auto& Exp : Exps) {
        std::string place = newVar();
        result += TransExp(Exp, vtable, ftable, place);
        args.push_back(place);
    }
    return result;
}

// Generates a new temporary variable (Phase A)
std::string Code_Generator::newVar() {
    static int varCount = 0;
    return fmt::format("t{}", varCount++);
}

// Generates a new label for conditional jumps (Phase A)
std::string Code_Generator::newLabel() {
    static int labelCount = 0;
    return fmt::format("L{}", labelCount++);
}


// New function to handle the entire program

// std::string Code_Generator::TransProg(std::shared_ptr<node> Prog, sym_table_type &vtable, sym_table_type &ftable)
// {
//     std::string code = "BEGIN PROGRAM\n";
//     // Handle global variables
//     if (hasChild(Prog, "GLOBVARS")) {
//         code += TransGlobVars(getChild(Prog, 1), vtable);
//     }
//     // Handle main algorithm
//     code += TransAlgo(getChild(Prog, 2), vtable, ftable);
//     // Handle functions
//     if (hasChild(Prog, "FUNCTIONS")) {
//         code += TransFunctions(getChild(Prog, 3), vtable, ftable);
//     }
//     code += "END PROGRAM\n";
//     return code;
// }

// New function to handle global variables
// std::string Code_Generator::TransGlobVars(std::shared_ptr<node> GlobVars, sym_table_type &vtable)
// {
//     std::string code = "GLOBAL VARIABLES\n";
//     for (size_t i = 0; i < GlobVars->get_children().size(); ++i) {
//         auto var = getChild(GlobVars, i);
//         std::string type = var->get_child(0)->WORD; // VTYP
//         std::string name = var->get_child(1)->WORD; // VNAME
//         code += fmt::format("{} {}\n", type, name);
//         vtable[name] = type; // Add to symbol table
//     }
//     return code;
// }

// // New function to handle functions
// std::string Code_Generator::TransFunctions(std::shared_ptr<node> Functions, sym_table_type &vtable, sym_table_type &ftable)
// {
//     std::string code = "FUNCTIONS\n";
//     for (size_t i = 0; i < Functions->get_children().size(); ++i) {
//         auto func = getChild(Functions, i);
//         code += TransFunction(func, vtable, ftable);
//     }
//     return code;
// }

// // New function to handle a single function
// std::string Code_Generator::TransFunction(std::shared_ptr<node> Function, sym_table_type &vtable, sym_table_type &ftable)
// {
//     std::string header = TransHeader(getChild(Function, 0), vtable, ftable); // Assuming HEADER is the first child
//     std::string body = TransBody(getChild(Function, 1), vtable, ftable); // Assuming BODY is the second child
//     return header + body;
// }

// // New function to handle function headers
// std::string Code_Generator::TransHeader(std::shared_ptr<node> Header, sym_table_type &vtable, sym_table_type &ftable)
// {
//     std::string returnType = getChild(Header, 0)->WORD; // Return type
//     std::string name = getChild(Header, 1)->WORD; // FNAME
//     std::string params = ""; // Handle parameters
//     for (size_t i = 2; i < Header->get_children().size(); ++i) {
//         params += getChild(Header, i)->WORD + " ";
//     }
//     ftable[name] = returnType; // Add to function table
//     return fmt::format("FUNCTION {} {}({})\n", returnType, name, params);
// }

// New function to handle the main algorithm
// std::string Code_Generator::TransAlgo(std::shared_ptr<node> Algo, sym_table_type &vtable, sym_table_type &ftable)
// {
//     std::string code = "BEGIN ALGORITHM\n";
//     for (size_t i = 0; i < Algo->get_children().size(); ++i) {
//         auto instruc = getChild(Algo, i);
//         code += TransStat(instruc, vtable, ftable);
//     }
//     code += "END ALGORITHM\n";
//     return code;
// }


// Utility function to check if a node has a child with a specific class
// bool Code_Generator::hasChild(const std::shared_ptr<node>& n, const std::string& child_class) {
//     try {
//         for (size_t i = 0; i < n->get_children().size(); ++i) {
//             if (n->get_child(i)->CLASS == child_class) {
//                 return true;
//             }
//         }
//     } catch (const std::exception& e) {
//         // Handle exception if get_child throws
//     }
//     return false;
// }

// Utility function to get a child node by index
// std::shared_ptr<node> Code_Generator::getChild(const std::shared_ptr<node>& n, size_t index) {
//     try {
//         return n->get_child(index);
//     } catch (const std::out_of_range& e) {
//         // Handle out of range error
//         return nullptr;
//     }
// }
