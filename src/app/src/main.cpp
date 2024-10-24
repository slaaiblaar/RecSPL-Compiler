// #include "include/pugixml/pugixml.hpp"
// #include <pugixml/pugixml.hpp>
#include "pugixml.hpp"
#include "automata.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "scope_checker.hpp"
#include "ast_node.hpp"
// #include
#include <iostream>
#include <fmt/core.h>
#include "testing.hpp"
#include <thread>
#include <fstream>

void test_function(int num)
{
    Tester test;
    test.cfg_file = fmt::format("CFG{}.xml", num);
    test.run_tests(num);
}

#include "code_generator.hpp"
#include "ast_node.hpp"
#include <iostream>
#include <memory>

// std::shared_ptr<node> build_complex_test_ast() {
//     // Root node representing the program (PROG)
//     auto root = std::make_shared<node>();
//     root->CLASS = "PROG";
//     root->WORD = "main";

//     // ALGO node representing "begin-end"
//     auto algo_node = std::make_shared<node>();
//     algo_node->CLASS = "ALGO";
//     algo_node->WORD = "begin-end";

//     // First instruction: Assignment (x = 10)
//     auto instruc1 = std::make_shared<node>();
//     instruc1->CLASS = "INSTRUC";

//     auto assign_cmd = std::make_shared<node>();
//     assign_cmd->CLASS = "ASSIGN";
//     assign_cmd->WORD = "=";

//     // Variable node for 'x'
//     auto x_var = std::make_shared<node>();
//     x_var->CLASS = "VID";
//     x_var->WORD = "x";

//     // Literal node for '10'
//     auto ten_literal = std::make_shared<node>();
//     ten_literal->CLASS = "LITERAL";
//     ten_literal->WORD = "10";

//     assign_cmd->add_child(x_var, 0);    // Left-hand side of assignment
//     assign_cmd->add_child(ten_literal, 1);  // Right-hand side of assignment
//     instruc1->add_child(assign_cmd, 0);

//     // Second instruction: Conditional Branch (if x > 5 then ...)
//     auto instruc2 = std::make_shared<node>();
//     instruc2->CLASS = "INSTRUC";

//     auto branch_cmd = std::make_shared<node>();
//     branch_cmd->CLASS = "BRANCH";
//     branch_cmd->WORD = "if";

//     // Condition: x > 5
//     auto cond = std::make_shared<node>();
//     cond->CLASS = "SIMPLE";
//     cond->WORD = "grt";  // grt for "greater than"

//     auto x_cond = std::make_shared<node>();
//     x_cond->CLASS = "VID";
//     x_cond->WORD = "x";

//     auto five_literal = std::make_shared<node>();
//     five_literal->CLASS = "LITERAL";
//     five_literal->WORD = "5";

//     cond->add_child(x_cond, 0);   // First operand (x)
//     cond->add_child(five_literal, 1);  // Second operand (5)

//     // Then-part: print 42
//     auto then_algo = std::make_shared<node>();
//     then_algo->CLASS = "ALGO";

//     auto then_instruc = std::make_shared<node>();
//     then_instruc->CLASS = "INSTRUC";

//     auto print_cmd_42 = std::make_shared<node>();
//     print_cmd_42->CLASS = "COMMAND";
//     print_cmd_42->WORD = "print";

//     auto literal_42 = std::make_shared<node>();
//     literal_42->CLASS = "LITERAL";
//     literal_42->WORD = "42";

//     print_cmd_42->add_child(literal_42, 0);
//     then_instruc->add_child(print_cmd_42, 0);
//     then_algo->add_child(then_instruc, 0);

//     // Else-part: print 0
//     auto else_algo = std::make_shared<node>();
//     else_algo->CLASS = "ALGO";

//     auto else_instruc = std::make_shared<node>();
//     else_instruc->CLASS = "INSTRUC";

//     auto print_cmd_0 = std::make_shared<node>();
//     print_cmd_0->CLASS = "COMMAND";
//     print_cmd_0->WORD = "print";

//     auto literal_0 = std::make_shared<node>();
//     literal_0->CLASS = "LITERAL";
//     literal_0->WORD = "0";

//     print_cmd_0->add_child(literal_0, 0);
//     else_instruc->add_child(print_cmd_0, 0);
//     else_algo->add_child(else_instruc, 0);

//     // Build the complete branch structure
//     branch_cmd->add_child(cond, 0);         // Condition
//     branch_cmd->add_child(then_algo, 1);    // Then-part
//     branch_cmd->add_child(else_algo, 2);    // Else-part

//     instruc2->add_child(branch_cmd, 0);

//     // Add both instructions to the ALGO node
//     algo_node->add_child(instruc1, 0);  // Assignment
//     algo_node->add_child(instruc2, 1);  // Branch

//     // Add ALGO to the root (PROG)
//     root->add_child(algo_node, 0);

//     return root;  // Return the fully built AST
// }

std::shared_ptr<node> build_complex_test_ast()
{
    // Root node representing the program (PROG)
    auto root = std::make_shared<node>();
    root->CLASS = "PROG";
    root->WORD = "main";

    // ALGO node representing "begin-end"
    auto algo_node = std::make_shared<node>();
    algo_node->CLASS = "ALGO";
    algo_node->WORD = "begin-end";

    // First instruction: Assignment (x = 10)
    auto instruc1 = std::make_shared<node>();
    instruc1->CLASS = "INSTRUC";

    auto assign_cmd = std::make_shared<node>();
    assign_cmd->CLASS = "ASSIGN";
    assign_cmd->WORD = "=";

    // Variable node for 'x'
    auto x_var = std::make_shared<node>();
    x_var->CLASS = "VID";
    x_var->WORD = "x";

    // Literal node for '10'
    auto ten_literal = std::make_shared<node>();
    ten_literal->CLASS = "LITERAL";
    ten_literal->WORD = "10";

    assign_cmd->add_child(x_var, 0);       // Left-hand side of assignment
    assign_cmd->add_child(ten_literal, 1); // Right-hand side of assignment
    instruc1->add_child(assign_cmd, 0);

    // Second instruction: Conditional Branch (if x > 5 then ...)
    auto instruc2 = std::make_shared<node>();
    instruc2->CLASS = "INSTRUC";

    auto branch_cmd = std::make_shared<node>();
    branch_cmd->CLASS = "BRANCH";
    branch_cmd->WORD = "if";

    // Condition: x > 5
    auto cond = std::make_shared<node>();
    cond->CLASS = "SIMPLE";
    cond->WORD = "grt"; // grt for "greater than"

    auto x_cond = std::make_shared<node>();
    x_cond->CLASS = "VID";
    x_cond->WORD = "x";

    auto five_literal = std::make_shared<node>();
    five_literal->CLASS = "LITERAL";
    five_literal->WORD = "5";

    cond->add_child(x_cond, 0);       // First operand (x)
    cond->add_child(five_literal, 1); // Second operand (5)

    // Then-part: print 42
    auto then_algo = std::make_shared<node>();
    then_algo->CLASS = "ALGO";

    auto then_instruc = std::make_shared<node>();
    then_instruc->CLASS = "INSTRUC";

    auto print_cmd_42 = std::make_shared<node>();
    print_cmd_42->CLASS = "COMMAND";
    print_cmd_42->WORD = "print";

    auto literal_42 = std::make_shared<node>();
    literal_42->CLASS = "LITERAL";
    literal_42->WORD = "42";

    print_cmd_42->add_child(literal_42, 0);
    then_instruc->add_child(print_cmd_42, 0);
    then_algo->add_child(then_instruc, 0);

    // Else-part: print 0
    auto else_algo = std::make_shared<node>();
    else_algo->CLASS = "ALGO";

    auto else_instruc = std::make_shared<node>();
    else_instruc->CLASS = "INSTRUC";

    auto print_cmd_0 = std::make_shared<node>();
    print_cmd_0->CLASS = "COMMAND";
    print_cmd_0->WORD = "print";

    auto literal_0 = std::make_shared<node>();
    literal_0->CLASS = "LITERAL";
    literal_0->WORD = "0";

    print_cmd_0->add_child(literal_0, 0);
    else_instruc->add_child(print_cmd_0, 0);
    else_algo->add_child(else_instruc, 0);

    // Build the complete branch structure
    branch_cmd->add_child(cond, 0);      // Condition
    branch_cmd->add_child(then_algo, 1); // Then-part
    branch_cmd->add_child(else_algo, 2); // Else-part

    instruc2->add_child(branch_cmd, 0);

    // Add both instructions to the ALGO node
    algo_node->add_child(instruc1, 0); // Assignment
    algo_node->add_child(instruc2, 1); // Branch

    // Add ALGO to the root (PROG)
    root->add_child(algo_node, 0);

    return root; // Return the fully built AST
}

void print_ast(std::shared_ptr<node> n, int depth = 0)
{
    if (!n)
        return;
    std::cout << std::string(depth, ' ') << "CLASS: " << n->CLASS << ", WORD: " << n->WORD << std::endl;
    for (auto &child : n->get_children())
    {
        print_ast(child, depth + 2); // Recursively print children with increased indentation
    }
}

int main(int argc, char *argv[])
{
    std::string dfa_test_strings[] = {
        "main", "num", "text", "begin", "end", "skip", "halt", "print",
        "<input", "=", "(", ",", ")", "if", "then", "else", "not", "sqrt", "or",
        "and", "eq", "grt", "add", "sub", "mul", "div", "void", "{", "}",
        "\"Hellowor\"", "V_hello", "F_world", "-0.123", "1.01", "0", "123",
        "-1"};
    // std::srand(std::time(0));
    // Scope_Checker SC;
    // // SC.testScopeChecker(10);
    // for (int i = 0; i < 10; ++i)
    // {
    //     Lexer luthor;
    //     luthor.read_input(fmt::format("./code_file{}.txt", i));
    //     std::cout << "INPUT READ\n";
    //     luthor.lex(true);

    //     std::cout << "LEXED\n";
    //     Parser p;
    //     std::cout << "input file " << i << "\n";
    //     p.parse(fmt::format("./code_file{}.txt", i));
    //     // std::cout << "PARSED\n";
    // }

    std::cout << "CODE GENERATOR TESTING:\n\n";
    auto ast_root = build_complex_test_ast();

    // Print the AST to verify its structure
    std::cout << "Printing AST structure:\n";
    print_ast(ast_root);

    // Initialize the code generator
    Code_Generator generator;

    // Phase A: Generate intermediate code
    std::cout << "Generating Intermediate Code...\n";
    std::string intermediate_code = generator.generate(ast_root);
    if (intermediate_code.empty())
    {
        std::cerr << "Error: No intermediate code was generated.\n";
    }
    else
    {
        std::cout << "Intermediate Code:\n"
                  << intermediate_code << "\n";
    }

    // Phase B: Generate final executable code
    std::cout << "Generating Final Code...\n";
    std::string final_code = generator.generate_final(ast_root);
    if (final_code.empty())
    {
        std::cerr << "Error: No final code was generated.\n";
    }
    else
    {
        std::cout << "Final Code:\n"
                  << final_code << "\n";
    }

    ast_root->clear_node();

    if (argc == 1)
    {
        std::thread t1(test_function, 1);
        // std::thread t2(test_function, 2);
        // std::thread t3(test_function, 3);
        // std::thread t4(test_function, 4);
        t1.join();
        // t2.join();
        // t3.join();
        // t4.join();
        // int num_threads = 4;
        // std::vector<std::shared_ptr<std::thread>> thread_vector;
        // for (int i = 1; i <= num_threads; ++i)
        // {
        //     pugi::xml_document thread_doc;
        //     if (!thread_doc.load_file(fmt::format("CFG{}.xml", i).c_str()))
        //     {
        //         pugi::xml_document original_doc;
        //         if (!original_doc.load_file("CFG.xml"))
        //         {
        //             std::cout << "Cannot load CFG file\n";
        //         }
        //         original_doc.save_file(fmt::format("CFG{}.xml", i).c_str());
        //     }
        //     thread_vector.push_back(std::make_shared<std::thread>(std::thread(test_function, i)));
        // }

        // for (auto t : thread_vector)
        // {
        //     t->join();
        // }
    }
    else if (argc > 1)
    {
        // Intermediate code generation test:
        std::string input_file(argv[1]);
        std::cout << "Phase A code generation test:" << std::endl;
        // test_code_generation();
        Lexer l("CFG.xml");
        // l.read_input("thread_1_code_file0.txt");
        l.read_input(input_file);
        std::ifstream i_file(input_file);
        if (!i_file.good())
        {
            std::cout << input_file << " does not exist\n";
            return 1;
        }
        if (!l.lex(false, "token_stream.xml"))
        {
            // std::cout << "Lexing failed\n";
            return 1;
        }
        Parser p("CFG.xml", input_file);
        std::shared_ptr<node> root = p.parse("ast.xml");
        if (root->NAME == "ERROR")
        {
            std::cout << "Parsing failed\n";
            return 1;
        }
        std::ofstream parsed_code("parsed_code_file.txt");
        parsed_code << root->print_code(0);
        parsed_code.close();
        Scope_Checker s(root, "CFG.xml");
        s.run_scope_checker();
        if (s.error_messages.size() > 0)
        {
            std::cout << "Scope checker failed\n";
            for (auto e : s.error_messages)
            {
                std::cout << e.first;
            }
            return 1;
        }
        std::ofstream s_tree("ast_scope.xml");
        s_tree << root->printnode(0, "main");
        Type_Checker t(input_file);
        t.check(root);
        std::cout << root->print_code(0, true);
        if (t.type_errors.size() > 0)
        {
            std::cout << "Type checker failed\n";
            for (auto e : t.type_errors)
            {
                std::cout << e.first;
            }
            return 1;
        }
        else
        {
            std::cout << "Type checker succeeded\n";
        }
    }
    return 0;
}