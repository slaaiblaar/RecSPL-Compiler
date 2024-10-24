#include "code_generator.hpp"
#include <fmt/core.h>
#include <sstream>
#include <iostream>

// Simulated Call Stack (Phase B)
std::vector<std::string> call_stack;

Code_Generator::Code_Generator() {
    std::cout << "Code Generator Initialized\n";
}

// Phase A: Generates intermediate code for the AST starting from the root node
std::string Code_Generator::generate(std::shared_ptr<node> root) {
    std::string code = TransStat(root, vtable, ftable);
    std::cout << "Intermediate Code: " << code << std::endl;
    return code;
}

// Phase B: Generates final executable code from the intermediate code
std::string Code_Generator::generate_final(std::shared_ptr<node> root) {
    std::string intermediate_code = generate(root);
    return generate_executable(intermediate_code);
}

// Phase B: Converts intermediate code into BASIC target code
std::string Code_Generator::generate_executable(const std::string& intermediate_code) {
    std::stringstream final_code;
    std::vector<std::string> lines;
    std::string line;
    int line_number = 10; // BASIC programs start at line 10, increment by 10

    // Parse the intermediate code into individual lines
    std::istringstream iss(intermediate_code);
    while (std::getline(iss, line)) {
        lines.push_back(line);
    }

    // Process each line of intermediate code into BASIC code
    for (const auto& code_line : lines) {
        if (code_line.find("CALL") != std::string::npos) {
            handle_function_call(final_code, code_line, line_number);
        } else if (code_line.find("RETURN") != std::string::npos) {
            handle_return(final_code, code_line, line_number);
        } else {
            // General instructions such as assignment, arithmetic, etc.
            final_code << line_number << " " << code_line << "\n";
        }
        line_number += 10;  // Increment line numbers by 10
    }

    return final_code.str();
}

// Phase B: Handles function calls in BASIC code
void Code_Generator::handle_function_call(std::stringstream& final_code, const std::string& call_instruction, int& line_number) {
    size_t start = call_instruction.find("CALL ") + 5;
    size_t open_paren = call_instruction.find("(", start);
    size_t close_paren = call_instruction.find(")", open_paren);

    std::string function_name = call_instruction.substr(start, open_paren - start);
    std::string args = call_instruction.substr(open_paren + 1, close_paren - open_paren - 1);

    // Generate BASIC code for the function call
    std::istringstream arg_stream(args);
    std::string arg;
    final_code << line_number << " REM Push arguments onto the stack\n";
    while (std::getline(arg_stream, arg, ',')) {
        final_code << line_number << " PUSH " << arg << "\n";
        line_number += 10;
    }

    // Simulate the function call with GOSUB
    final_code << line_number << " GOSUB " << function_name << "\n";
}

void Code_Generator::handle_return(std::stringstream& final_code, const std::string& return_instruction, int& line_number) {
    size_t start = return_instruction.find("RETURN ") + 7;
    std::string return_value = return_instruction.substr(start);

    // Generate BASIC code for return
    final_code << line_number << " REM Return value to caller\n";
    final_code << line_number << " RETURN\n";
}

std::string Code_Generator::TransExp(std::shared_ptr<node> Exp, sym_table_type& vtable, sym_table_type& ftable, std::string& place) {
    std::cout << "Translating EXP with CLASS: " << Exp->CLASS << ", WORD: " << Exp->WORD << std::endl;
    
    if (Exp->CLASS == "LITERAL") {
        std::string v = Exp->WORD;
        return fmt::format("{} := {}\n", place, v);
    }
    if (Exp->CLASS == "VID") {
        std::string vid = Exp->WORD; // Directly use VID from the node
        return fmt::format("{} := {}\n", place, vid);
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


std::string Code_Generator::TransStat(std::shared_ptr<node> Stat, sym_table_type& vtable, sym_table_type& ftable) {
    std::cout << "Translating STAT with CLASS: " << Stat->CLASS << ", WORD: " << Stat->WORD << std::endl;
    
    // Handle PROG node: delegate to children (ALGO, FUNCTIONS, etc.)
    if (Stat->CLASS == "PROG") {
        std::string algo_code;
        if (Stat->num_children() > 0) {
            algo_code = TransStat(Stat->get_child(0), vtable, ftable);  // Assume first child is ALGO
        }
        return algo_code + "STOP\n";
    }

    if (Stat->CLASS == "ALGO") {
        if (Stat->num_children() > 0) {
            return TransStat(Stat->get_child(0), vtable, ftable);  // Translate INSTRUC
        }
        return "REM END\n";  // Empty ALGO block
    }

    if (Stat->CLASS == "INSTRUC") {
        if (Stat->num_children() > 0) {
            return TransStat(Stat->get_child(0), vtable, ftable);  // Translate COMMAND
        }
    }

    if (Stat->CLASS == "COMMAND") {
        if (Stat->WORD == "print") {
            // Handle print command
            std::string code = "PRINT ";
            if (Stat->num_children() > 0) {
                // Directly append the atomic value (literal or variable)
                std::string arg = Stat->get_child(0)->WORD;  // Get the literal or variable name directly
                return code + arg + "\n";  // Corrected to only append the value
            }
        }
    }


    if (Stat->CLASS == "ASSIGN") {
        std::string place = newVar();
        std::string code1 = TransExp(Stat->get_child(1), vtable, ftable, place);
        std::string x = Stat->get_child(0)->WORD;  // Use VID directly from node->WORD
        return code1 + fmt::format("{} := {}\n", x, place);
    }
    if (Stat->CLASS == "BRANCH") {
        std::string label1 = newLabel();
        std::string label2 = newLabel();
        std::string code1 = TransCond(Stat->get_child(0), label1, label2, vtable, ftable);
        std::string code2 = TransStat(Stat->get_child(1), vtable, ftable);
        return code1 + fmt::format("LABEL {}\n", label1) + code2 + fmt::format("LABEL {}\n", label2);
    }
    if (Stat->CLASS == "RETURN") {
        std::string ret_val = Stat->get_child(0)->WORD; // Return value directly from node->WORD
        return fmt::format("RETURN {}\n", ret_val);
    }
    return "";
}


std::string Code_Generator::TransCond(std::shared_ptr<node> Cond, const std::string& labelTrue, const std::string& labelFalse, sym_table_type& vtable, sym_table_type& ftable) {
    std::string t1 = newVar();
    std::string t2 = newVar();
    std::string code1 = TransExp(Cond->get_child(0), vtable, ftable, t1);
    std::string code2 = TransExp(Cond->get_child(1), vtable, ftable, t2);
    std::string op = Cond->WORD;
    return code1 + code2 + fmt::format("IF {} {} {} THEN GOTO {} ELSE GOTO {}\n", t1, op, t2, labelTrue, labelFalse);
}

std::string Code_Generator::TransExps(std::vector<std::shared_ptr<node>> Exps, sym_table_type& vtable, sym_table_type& ftable, std::vector<std::string>& args) {
    std::string result;
    for (auto& Exp : Exps) {
        std::string place = newVar();
        result += TransExp(Exp, vtable, ftable, place);
        args.push_back(place);
    }
    return result;
}

std::string Code_Generator::newVar() {
    static int varCount = 0;
    return fmt::format("t{}", varCount++);
}

std::string Code_Generator::newLabel() {
    static int labelCount = 0;
    return fmt::format("L{}", labelCount++);
}

std::string Code_Generator::new_frame()
{
    static int frameCount = 0;
    return fmt::format("frame{}", frameCount++);
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


