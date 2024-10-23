// #include "code_generator.hpp"
// #include <fmt/core.h>
// #include <sstream>
// #include <iostream>

// // Phase B: Simulated Call Stack (for function calls)
// std::vector<std::string> call_stack;

// Code_Generator::Code_Generator() {
//     std::cout << "Code Generator Initialized\n";
// }

// // Phase A: Generates intermediate code for the AST starting from the root node
// std::string Code_Generator::generate(std::shared_ptr<node> root) {
//     return TransStat(root, vtable, ftable);  // Start translation from the root statement
// }

// // Phase B: Generates final executable code from the intermediate code
// std::string Code_Generator::generate_final(std::shared_ptr<node> root) {
//     std::string intermediate_code = generate(root);  // Generate intermediate code (Phase A)
//     return generate_executable(intermediate_code);   // Convert to executable code (Phase B)
// }

// // Phase B: Converts intermediate code into executable code
// std::string Code_Generator::generate_executable(const std::string& intermediate_code) {
//     std::stringstream final_code;
//     std::vector<std::string> lines;
//     std::string line;

//     // Split the intermediate code into individual lines
//     std::istringstream iss(intermediate_code);
//     while (std::getline(iss, line)) {
//         lines.push_back(line);
//     }

//     // Process each line of intermediate code into executable form
//     for (const auto& code_line : lines) {
//         if (code_line.find("CALL") != std::string::npos) {
//             // Handle function calls
//             handle_function_call(final_code, code_line);
//         } else if (code_line.find("RETURN") != std::string::npos) {
//             // Handle return statement
//             handle_return(final_code, code_line);
//         } else {
//             // Handle general instructions (assignment, arithmetic)
//             final_code << code_line << "\n";
//         }
//     }

//     return final_code.str();
// }

// // Phase B: Simulates a function call by pushing arguments onto the stack and setting up the call
// void Code_Generator::handle_function_call(std::stringstream& final_code, const std::string& call_instruction) {
//     size_t start = call_instruction.find("CALL ") + 5;
//     size_t open_paren = call_instruction.find("(", start);
//     size_t close_paren = call_instruction.find(")", open_paren);

//     std::string function_name = call_instruction.substr(start, open_paren - start);
//     std::string args = call_instruction.substr(open_paren + 1, close_paren - open_paren - 1);

//     // Simulate pushing the arguments onto the stack
//     final_code << "// Push arguments onto the stack\n";
//     std::istringstream arg_stream(args);
//     std::string arg;
//     while (std::getline(arg_stream, arg, ',')) {
//         final_code << "PUSH " << arg << "\n";
//     }

//     // Simulate the function call
//     final_code << "// Call the function\n";
//     final_code << "CALL " << function_name << "\n";

//     // After the function call, pop the result from the stack (if needed)
//     final_code << "// Pop the result from the stack\n";
//     final_code << "POP result\n";
// }

// // Phase B: Simulates a return statement by managing the call stack and returning control to the caller
// void Code_Generator::handle_return(std::stringstream& final_code, const std::string& return_instruction) {
//     size_t start = return_instruction.find("RETURN ") + 7;
//     std::string return_value = return_instruction.substr(start);

//     // Simulate returning the value
//     final_code << "// Return value to the caller\n";
//     final_code << "RETURN " << return_value << "\n";

//     // Pop the call stack
//     final_code << "// Pop the current function's stack frame\n";
//     final_code << "POP_FRAME\n";
// }

// // Phase A: Translates an expression node to intermediate code
// std::string Code_Generator::TransExp(std::shared_ptr<node> Exp, sym_table_type& vtable, sym_table_type& ftable, std::string& place) {
//     if (Exp->CLASS == "LITERAL") {
//         std::string v = Exp->WORD;  // Assuming literal value is in WORD
//         return fmt::format("{} := {}\n", place, v);
//     }
//     if (Exp->CLASS == "VID") {  // Variable identifier (VID)
//         std::string x = Exp->WORD;
//         return fmt::format("{} := {}\n", place, x);
//     }
//     if (Exp->CLASS == "BINOP") {
//         std::string place1 = newVar();
//         std::string place2 = newVar();
//         std::string code1 = TransExp(Exp->get_child(0), vtable, ftable, place1);
//         std::string code2 = TransExp(Exp->get_child(1), vtable, ftable, place2);
//         std::string op = Exp->WORD;  // Binary operation
//         return code1 + code2 + fmt::format("{} := {} {} {}\n", place, place1, op, place2);
//     }

//     // Handle function calls (Phase A)
//     if (Exp->CLASS == "FNAME") {
//         std::vector<std::string> args;
//         std::string code1 = TransExps(Exp->get_children(), vtable, ftable, args);
//         std::ostringstream argsStream;
//         for (size_t i = 0; i < args.size(); ++i) {
//             argsStream << args[i];
//             if (i != args.size() - 1) {
//                 argsStream << ", ";
//             }
//         }
//         return code1 + fmt::format("{} := CALL {}({})\n", place, Exp->WORD, argsStream.str());
//     }

//     return "";
// }

// // Phase A: Translates a statement node to intermediate code
// std::string Code_Generator::TransStat(std::shared_ptr<node> Stat, sym_table_type& vtable, sym_table_type& ftable) {
//     if (Stat->CLASS == "ASSIGN") {
//         std::string place = newVar();
//         std::string code1 = TransExp(Stat->get_child(1), vtable, ftable, place);  // Right-hand side expression
//         std::string x = Stat->get_child(0)->WORD;  // Left-hand side (variable name)
//         return code1 + fmt::format("{} := {}\n", x, place);
//     }

//     if (Stat->CLASS == "BRANCH") {
//         std::string label1 = newLabel();
//         std::string label2 = newLabel();
//         std::string code1 = TransCond(Stat->get_child(0), label1, label2, vtable, ftable);  // Condition
//         std::string code2 = TransStat(Stat->get_child(1), vtable, ftable);  // Then branch
//         std::string code3 = TransStat(Stat->get_child(2), vtable, ftable);  // Else branch
//         return code1 + fmt::format("LABEL {}\n", label1) + code2 + fmt::format("GOTO {}\nLABEL {}\n", label2, label2) + code3;
//     }

//     if (Stat->CLASS == "RETURN") {
//         std::string place = newVar();
//         std::string code1 = TransExp(Stat->get_child(0), vtable, ftable, place);
//         return code1 + fmt::format("RETURN {}\n", place);
//     }

//     return "";
// }

// // Phase A: Translates a condition node to intermediate code
// std::string Code_Generator::TransCond(std::shared_ptr<node> Cond, const std::string& labelTrue, const std::string& labelFalse, sym_table_type& vtable, sym_table_type& ftable) {
//     std::string t1 = newVar();
//     std::string t2 = newVar();
//     std::string code1 = TransExp(Cond->get_child(0), vtable, ftable, t1);
//     std::string code2 = TransExp(Cond->get_child(1), vtable, ftable, t2);
//     std::string op = Cond->WORD;  // Relational operator
//     return code1 + code2 + fmt::format("IF {} {} {} THEN GOTO {} ELSE GOTO {}\n", t1, op, t2, labelTrue, labelFalse);
// }

// // Phase A: Translates multiple expressions (for function arguments)
// // Phase A: Translates multiple expressions (for function arguments)
// std::string Code_Generator::TransExps(std::vector<std::shared_ptr<node>> Exps, sym_table_type& vtable, sym_table_type& ftable, std::vector<std::string>& args) {
//     std::string result;
//     for (auto& Exp : Exps) {
//         std::string place = newVar();
//         result += TransExp(Exp, vtable, ftable, place);
//         args.push_back(place);
//     }
//     return result;
// }

// // Phase A: Translates function declarations (DECL)
// std::string Code_Generator::TransDecl(std::shared_ptr<node> Decl, sym_table_type& vtable, sym_table_type& ftable) {
//     std::string header = TransHeader(Decl->get_child(0), vtable, ftable);  // Translate header (HEADER)
//     std::string body = TransBody(Decl->get_child(1), vtable, ftable);      // Translate body (BODY)
//     return header + body;
// }

// // Phase A: Translates the function header (HEADER)
// std::string Code_Generator::TransHeader(std::shared_ptr<node> Header, sym_table_type& vtable, sym_table_type& ftable) {
//     std::string return_type = Header->get_child(0)->WORD;  // Return type (num or void)
//     std::string function_name = Header->get_child(1)->WORD;  // Function name (FNAME)
//     std::string param1 = Header->get_child(2)->WORD;  // First parameter (VID)
//     std::string param2 = Header->get_child(3)->WORD;  // Second parameter (VID)
//     std::string param3 = Header->get_child(4)->WORD;  // Third parameter (VID)

//     // Add function parameters to the ftable
//     ftable[function_name] = return_type;
//     return fmt::format("{} {}({},{},{})\n", return_type, function_name, param1, param2, param3);
// }

// // Phase A: Translates the function body (BODY)
// std::string Code_Generator::TransBody(std::shared_ptr<node> Body, sym_table_type& vtable, sym_table_type& ftable) {
//     std::string locals = TransLocVars(Body->get_child(0), vtable, ftable);  // Translate local variables (LOCVARS)
//     std::string algo = TransStat(Body->get_child(1), vtable, ftable);       // Translate instructions (ALGO)
//     return fmt::format("{{\n{}\n{}\n}}\n", locals, algo);
// }

// // Phase A: Translates local variables (LOCVARS)
// std::string Code_Generator::TransLocVars(std::shared_ptr<node> LocVars, sym_table_type& vtable, sym_table_type& ftable) {
//     std::string vars = "";
//     for (auto& var : LocVars->get_children()) {
//         std::string type = var->get_child(0)->WORD;  // Variable type (VTYP)
//         std::string name = var->get_child(1)->WORD;  // Variable name (VID)
//         vtable[name] = type;  // Store type in vtable
//         vars += fmt::format("{} {};\n", type, name);
//     }
//     return vars;
// }

// // Utility: Generates a new temporary variable
// std::string Code_Generator::newVar() {
//     static int varCount = 0;
//     return fmt::format("t{}", varCount++);
// }

// // Utility: Generates a new label for conditional jumps
// std::string Code_Generator::newLabel() {
//     static int labelCount = 0;
//     return fmt::format("L{}", labelCount++);
// }

// // Utility: Generates a new stack frame (Phase B)
// std::string Code_Generator::new_frame() {
//     static int frameCount = 0;
//     return fmt::format("frame{}", frameCount++);
// }

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
    return TransStat(root, vtable, ftable); // Start generating code from the root node
}

// Phase B: Generates final executable code from the intermediate code
std::string Code_Generator::generate_final(std::shared_ptr<node> root)
{
    std::string intermediate_code = generate(root); // Generate intermediate code (Phase A)
    return generate_executable(intermediate_code);  // Convert to executable code (Phase B)
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
        // return code1 + fmt::format("{} := CALL {}({})\n", place, fname, fmt::join(args, ", "));
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
        return TransStat(Stat->get_child(0), vtable, ftable); // Handle COMMANDS like print, skip, etc.
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
    return code1 + code2 + fmt::format("IF {} {} {} THEN {} ELSE {}\n", t1, op, t2, labelTrue, labelFalse);
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
