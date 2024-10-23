#include "code_generator.hpp"
#include <fmt/core.h>
#include <sstream>
#include <iostream>

// Simulated Call Stack (Phase B)
std::vector<std::string> call_stack;

Code_Generator::Code_Generator()
{
    std::cout << "Code Generator Initialized\n";
}

// Phase A: Generates intermediate code for the AST starting from the root node
std::string Code_Generator::generate(std::shared_ptr<node> root)
{
    std::string code = TransStat(root, vtable, ftable);
    std::cout << "Intermediate Code: " << code << std::endl; // Debugging output
    return code;
}

// Phase B: Generates final executable code from the intermediate code
std::string Code_Generator::generate_final(std::shared_ptr<node> root)
{
    std::string intermediate_code = generate(root);
    std::cout << "Intermediate Code for Final Generation: " << intermediate_code << std::endl; // Debugging output
    return generate_executable(intermediate_code);
}

// Phase B: Converts intermediate code into executable code
std::string Code_Generator::generate_executable(const std::string &intermediate_code)
{
    std::stringstream final_code;
    std::vector<std::string> lines;
    std::string line;

    // Parse the intermediate code into individual lines
    std::istringstream iss(intermediate_code);
    while (std::getline(iss, line))
    {
        lines.push_back(line);
    }

    // Process each line of intermediate code into executable form
    for (const auto &code_line : lines)
    {
        if (code_line.find("CALL") != std::string::npos)
        {
            handle_function_call(final_code, code_line);
        }
        else if (code_line.find("RETURN") != std::string::npos)
        {
            handle_return(final_code, code_line);
        }
        else
        {
            final_code << code_line << "\n"; // General instructions
        }
    }

    return final_code.str();
}

// Phase B: Handles function calls in executable code
void Code_Generator::handle_function_call(std::stringstream &final_code, const std::string &call_instruction)
{
    size_t start = call_instruction.find("CALL ") + 5;
    size_t open_paren = call_instruction.find("(", start);
    size_t close_paren = call_instruction.find(")", open_paren);

    std::string function_name = call_instruction.substr(start, open_paren - start);
    std::string args = call_instruction.substr(open_paren + 1, close_paren - open_paren - 1);

    // Simulate pushing arguments onto the stack
    final_code << "// Push arguments onto the stack\n";
    std::istringstream arg_stream(args);
    std::string arg;
    while (std::getline(arg_stream, arg, ','))
    {
        final_code << "PUSH " << arg << "\n";
    }

    // Simulate the function call
    final_code << "// Call the function\n";
    final_code << "CALL " << function_name << "\n";

    // After the function call, pop the result from the stack
    final_code << "// Pop the result from the stack\n";
    final_code << "POP result\n";
}

// Phase B: Handles return statements in executable code
void Code_Generator::handle_return(std::stringstream &final_code, const std::string &return_instruction)
{
    size_t start = return_instruction.find("RETURN ") + 7;
    std::string return_value = return_instruction.substr(start);

    // Simulate returning the value
    final_code << "// Return value to the caller\n";
    final_code << "RETURN " << return_value << "\n";

    // Pop the call stack
    final_code << "// Pop the current function's stack frame\n";
    final_code << "POP_FRAME\n";
}



// Translates an expression node to intermediate code (Phase A)
std::string Code_Generator::TransExp(std::shared_ptr<node> Exp, sym_table_type &vtable, sym_table_type &ftable, std::string &place)
{
    if (Exp->CLASS == "LITERAL")
    {
        std::string v = Exp->WORD; // Literal (num or text)
        return fmt::format("{} := {}\n", place, v);
    }
    if (Exp->CLASS == "VID")
    {
        std::string x = Exp->WORD; // Variable ID is directly in WORD (VID)
        return fmt::format("{} := {}\n", place, x);
    }
    if (Exp->CLASS == "BINOP")
    {
        std::string place1 = newVar();
        std::string place2 = newVar();
        std::string code1 = TransExp(Exp->get_child(0), vtable, ftable, place1);
        std::string code2 = TransExp(Exp->get_child(1), vtable, ftable, place2);
        std::string op = Exp->WORD; // Binary operation
        return code1 + code2 + fmt::format("{} := {} {} {}\n", place, place1, op, place2);
    }

    // Handle function calls (Phase A)
    if (Exp->CLASS == "FNAME")
    {
        std::vector<std::string> args;
        std::string code1 = TransExps(Exp->get_children(), vtable, ftable, args);
        std::string fname = Exp->WORD; // Function name directly from WORD (FID)
        
        std::ostringstream args_stream;
        for (size_t i = 0; i < args.size(); ++i)
        {
            args_stream << args[i];
            if (i != args.size() - 1)
            {
                args_stream << ", "; // Add comma between arguments
            }
        }
        return code1 + fmt::format("{} := CALL {}({})\n", place, fname, args_stream.str());
    }
    return "";
}

// Translates a statement node to intermediate code (Phase A)
std::string Code_Generator::TransStat(std::shared_ptr<node> Stat, sym_table_type &vtable, sym_table_type &ftable)
{
    if (Stat->CLASS == "ASSIGN")
    {
        std::string place = newVar();
        std::string code1 = TransExp(Stat->get_child(1), vtable, ftable, place);
        std::string x = Stat->get_child(0)->WORD; // Variable name (VID in WORD)
        return code1 + fmt::format("{} := {}\n", x, place);
    }
    if (Stat->CLASS == "BRANCH")
    {
        std::string label1 = newLabel();
        std::string label2 = newLabel();
        std::string code1 = TransCond(Stat->get_child(0), label1, label2, vtable, ftable);
        std::string code2 = TransStat(Stat->get_child(1), vtable, ftable);
        return code1 + fmt::format("LABEL {}\n", label1) + code2 + fmt::format("LABEL {}\n", label2);
    }
    if (Stat->CLASS == "COMMAND")
    {
        if (Stat->get_child(0)->CLASS == "FNAME")
        {
            // Handle function call as a command
            return TransStat(Stat->get_child(0), vtable, ftable);
        }
        else if (Stat->get_child(0)->CLASS == "PRINT")
        {
            std::string place = newVar();
            std::string code1 = TransExp(Stat->get_child(1), vtable, ftable, place);
            return code1 + fmt::format("PRINT {}\n", place);
        }
        else if (Stat->get_child(0)->CLASS == "SKIP")
        {
            return "NOP\n"; // No operation
        }
        else if (Stat->get_child(0)->CLASS == "HALT")
        {
            return "HALT\n";
        }
        else if (Stat->get_child(0)->CLASS == "RETURN")
        {
            std::string place = newVar();
            std::string code1 = TransExp(Stat->get_child(1), vtable, ftable, place);
            return code1 + fmt::format("RETURN {}\n", place);
        }
    }
    return "";
}

// Translates a condition node to intermediate code (Phase A)
std::string Code_Generator::TransCond(std::shared_ptr<node> Cond, const std::string &labelTrue, const std::string &labelFalse, sym_table_type &vtable, sym_table_type &ftable)
{
    std::string t1 = newVar();
    std::string t2 = newVar();
    std::string code1 = TransExp(Cond->get_child(0), vtable, ftable, t1);
    std::string code2 = TransExp(Cond->get_child(1), vtable, ftable, t2);
    std::string op = Cond->WORD; // Relational operator (e.g., eq, grt)
    return code1 + code2 + fmt::format("IF {} {} {} THEN GOTO {} ELSE GOTO {}\n", t1, op, t2, labelTrue, labelFalse);
}

// Translates multiple expressions for function arguments
std::string Code_Generator::TransExps(std::vector<std::shared_ptr<node>> Exps, sym_table_type &vtable, sym_table_type &ftable, std::vector<std::string> &args)
{
    std::string result;
    for (auto &Exp : Exps)
    {
        std::string place = newVar();
        result += TransExp(Exp, vtable, ftable, place);
        args.push_back(place);
    }
    return result;
}

// Translates the function body (BODY) - Phase A
std::string Code_Generator::TransBody(std::shared_ptr<node> Body, sym_table_type &vtable, sym_table_type &ftable)
{
    std::string locals = TransLocVars(Body->get_child(0), vtable, ftable); // Local variables (LOCVARS)
    std::string algo = TransStat(Body->get_child(1), vtable, ftable);      // ALGO (instructions)
    return fmt::format("{{\n{}\n{}\n}}\n", locals, algo);
}

// Translates local variables (LOCVARS) - Phase A
std::string Code_Generator::TransLocVars(std::shared_ptr<node> LocVars, sym_table_type &vtable, sym_table_type &ftable)
{
    std::string vars;
    for (auto child : LocVars->get_children())
    {
        std::string type = child->get_child(0)->WORD; // Variable type (VTYP)
        std::string name = child->get_child(1)->WORD; // Variable name (VNAME)
        vtable[name] = type;                          // Add local variable to vtable
        vars += fmt::format("{} {};\n", type, name);
    }
    return vars;
}

// Generates a new temporary variable (Phase A)
std::string Code_Generator::newVar()
{
    static int varCount = 0;
    return fmt::format("t{}", varCount++);
}

// Generates a new label for conditional jumps (Phase A)
std::string Code_Generator::newLabel()
{
    static int labelCount = 0;
    return fmt::format("L{}", labelCount++);
}

// Generates a new simulated call stack frame (Phase B)
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
