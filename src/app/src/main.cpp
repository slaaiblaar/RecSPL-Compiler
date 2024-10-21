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
void test_function(int num)
{
    Tester test;
    test.cfg_file = fmt::format("CFG{}.xml", num);
    test.run_tests(num);
}

void test_code_generation() {
    Code_Generator cg;

    // Test 1: Simple Assignment (x := 5)
    std::cout << "Test 1: Simple Assignment (x := 5)" << std::endl;
    auto assign_node = std::make_shared<node>();
    assign_node->CLASS = "assign";

    auto lhs = std::make_shared<node>();  // Left-hand side, variable x
    lhs->CLASS = "id";
    lhs->WORD = "x";

    auto rhs = std::make_shared<node>();  // Right-hand side, number 5
    rhs->CLASS = "num";
    rhs->WORD = "5";

    assign_node->add_child(lhs, 0);
    assign_node->add_child(rhs, 1);

    cg.vtable["x"] = "x";  // Ensure x is in the vtable
    std::string output1 = cg.generate(assign_node);
    std::cout << output1 << std::endl;

    assign_node->clear_node();

    // Test 2: Binary Operation (y := x + 5)
    std::cout << "Test 2: Binary Operation (y := x + 5)" << std::endl;
    auto binop_node = std::make_shared<node>();
    binop_node->CLASS = "assign";

    auto lhs2 = std::make_shared<node>();  // Left-hand side, variable y
    lhs2->CLASS = "id";
    lhs2->WORD = "y";

    auto binop = std::make_shared<node>();  // Binary operation: x + 5
    binop->CLASS = "binop";
    binop->WORD = "+";  // Operation is addition

    auto binop_lhs = std::make_shared<node>();  // x
    binop_lhs->CLASS = "id";
    binop_lhs->WORD = "x";

    auto binop_rhs = std::make_shared<node>();  // 5
    binop_rhs->CLASS = "num";
    binop_rhs->WORD = "5";

    binop->add_child(binop_lhs, 0);
    binop->add_child(binop_rhs, 1);

    binop_node->add_child(lhs2, 0);  // y :=
    binop_node->add_child(binop, 1);  // x + 5

    cg.vtable["y"] = "y";  // Ensure y is in the vtable
    std::string output2 = cg.generate(binop_node);
    std::cout << output2 << std::endl;

    binop_node->clear_node();

    // Test 3: Function Call (z := my_function(5, x))
    std::cout << "Test 3: Function Call (z := my_function(5, x))" << std::endl;
    auto call_node = std::make_shared<node>();
    call_node->CLASS = "call";
    call_node->WORD = "my_function";  // Function name

    auto call_arg1 = std::make_shared<node>();
    call_arg1->CLASS = "num";
    call_arg1->WORD = "5";  // First argument: 5

    auto call_arg2 = std::make_shared<node>();
    call_arg2->CLASS = "id";
    call_arg2->WORD = "x";  // Second argument: x

    call_node->add_child(call_arg1, 0);
    call_node->add_child(call_arg2, 1);

    auto assign_call_node = std::make_shared<node>();
    assign_call_node->CLASS = "assign";

    auto lhs_call = std::make_shared<node>();  // Left-hand side, variable z
    lhs_call->CLASS = "id";
    lhs_call->WORD = "z";

    assign_call_node->add_child(lhs_call, 0);  // z :=
    assign_call_node->add_child(call_node, 1);  // my_function(5, x)

    cg.vtable["z"] = "z";  // Ensure z is in the vtable
    cg.ftable["my_function"][0] = "my_function";  // Ensure function is in the ftable
    std::string output3 = cg.generate(assign_call_node);
    std::cout << output3 << std::endl;

    assign_call_node->clear_node();

    // Test 4: If Statement (if x > 5 then y := 10)
    std::cout << "Test 4: If Statement (if x > 5 then y := 10)" << std::endl;
    auto if_node = std::make_shared<node>();
    if_node->CLASS = "if";

    auto cond = std::make_shared<node>();  // Condition: x > 5
    cond->CLASS = "binop";
    cond->WORD = ">";

    auto cond_lhs = std::make_shared<node>();  // x
    cond_lhs->CLASS = "id";
    cond_lhs->WORD = "x";

    auto cond_rhs = std::make_shared<node>();  // 5
    cond_rhs->CLASS = "num";
    cond_rhs->WORD = "5";

    cond->add_child(cond_lhs, 0);
    cond->add_child(cond_rhs, 1);

    auto if_body = std::make_shared<node>();  // y := 10
    if_body->CLASS = "assign";

    auto if_lhs = std::make_shared<node>();  // y
    if_lhs->CLASS = "id";
    if_lhs->WORD = "y";

    auto if_rhs = std::make_shared<node>();  // 10
    if_rhs->CLASS = "num";
    if_rhs->WORD = "10";

    if_body->add_child(if_lhs, 0);
    if_body->add_child(if_rhs, 1);

    if_node->add_child(cond, 0);  // if condition
    if_node->add_child(if_body, 1);  // then body

    std::string output4 = cg.generate(if_node);
    std::cout << output4 << std::endl;

    if_node->clear_node();
}

int main()
{
    std::string dfa_test_strings[] = {
        "main", "num", "text", "begin", "end", "skip", "halt", "print",
        "<input", "=", "(", ",", ")", "if", "then", "else", "not", "sqrt", "or",
        "and", "eq", "grt", "add", "sub", "mul", "div", "void", "{", "}",
        "\"Hellowor\"", "V_hello", "F_world", "-0.123", "1.01", "0", "123",
        "-1"};

    std::cout << "Testing scope checker" << std::endl;

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

    // Intermediate code generation test:
    std::cout << "Phase A code generation test:" << std::endl;
    test_code_generation();
    
    return 0;
}